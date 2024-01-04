#pragma once

#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12RenderQueue.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"
#include "Core/Foundation/Config.h"
namespace luna::graphics
{
	RENDER_API CONFIG_IMPLEMENT(LString, Render, RenderDeviceType, "Vulkan");
	RHIStagingBufferPool::RHIStagingBufferPool(RHIDevice* device) :mDevice(device), mResourcePoolCount(0), isPoolWorking(false)
	{
		mCopyFence = mDevice->CreateFence();
		mCopyCommandList = mDevice->CreateSinglePoolMultiCommondList(RHICmdListType::Copy);
		mRenderQueue = GenerateRenderQueue(RHIQueueType::eTransfer);
	}

	RHIStagingBuffer* RHIStagingBufferPool::CreateUploadBuffer(size_t dataLength, void* initData)
	{
		//申请一个上传buffer
		RHIBufferDesc resDesc;
		resDesc.mSize = dataLength;
		resDesc.mBufferUsage = RHIBufferUsage::TransferSrcBit;
		RHIStagingBuffer dstBuffer;
		dstBuffer.mBufferData = mDevice->CreateBufferExt(resDesc);
		dstBuffer.mBufferSize = resDesc.mSize;
		//为上传buffer开辟空间
		dstBuffer.mBufferData->BindMemory(RHIHeapType::Upload);
		//拷贝数据
		void* mapPointer = dstBuffer.mBufferData->Map();
		memcpy(mapPointer, initData, dataLength);
		dstBuffer.mBufferData->Unmap();
		//储存这个数据
		dstBuffer.mResourceId = -1;
		if (!mUnusedResourceId.empty())
		{
			dstBuffer.mResourceId= mUnusedResourceId.front();
			mUnusedResourceId.pop();
		}
		else
		{
			dstBuffer.mResourceId = mResourcePoolCount;
			mResourcePoolCount += 1;
		}
		mResourceWaitingCopy.insert({ dstBuffer.mResourceId ,dstBuffer });
		return &mResourceWaitingCopy[dstBuffer.mResourceId];
	}

	void RHIStagingBufferPool::UploadToDstResource(size_t dataLength, void* initData, RHIResource* dstResource, size_t offset_dst, std::function<void(RHICmdList* curCmdList, RHIStagingBuffer* srcBuffer)> copyCommand)
	{
		RHIStagingBuffer* srcBuffer;
		srcBuffer = CreateUploadBuffer(dataLength, initData);
		RHICmdList* curCmdList = mCopyCommandList->GetNewCmdList();
		if (dstResource->GetInitialState() != kCopyDest)
		{
			ResourceBarrierDesc dstBarrier;
			dstBarrier.mBarrierRes = dstResource;
			dstBarrier.mStateBefore = dstResource->GetInitialState();
			dstBarrier.mStateAfter = kCopyDest;
			dstBarrier.mMipLevels = dstResource->GetDesc().MipLevels;
			dstBarrier.mDepth = dstResource->GetDesc().DepthOrArraySize;
			curCmdList->ResourceBarrierExt(dstBarrier);
		}
		copyCommand(curCmdList, srcBuffer);
		ResourceBarrierDesc undoBarrier;
		undoBarrier.mBarrierRes = dstResource;
		undoBarrier.mStateBefore = kCopyDest;
		undoBarrier.mStateAfter = kCommon;
		undoBarrier.mMipLevels = dstResource->GetDesc().MipLevels;
		undoBarrier.mDepth = dstResource->GetDesc().DepthOrArraySize;
		curCmdList->ResourceBarrierExt(undoBarrier);
		curCmdList->CloseCommondList();
		mRenderQueue->ExecuteCommandLists(curCmdList);
		//记录当前buffer需要监控的拷贝指令fence
		size_t nextSignalValue = mCopyFence->GetNextSignalValue();
		mResourceCopying[nextSignalValue].push_back(mResourceWaitingCopy[srcBuffer->mResourceId]);
		mResourceWaitingCopy.erase(srcBuffer->mResourceId);
		mUnusedResourceId.push(srcBuffer->mResourceId);
		//插入一个桩
		mCopyFence->IncSignal(mRenderQueue);
		isPoolWorking = true;
	}

	void RHIStagingBufferPool::UploadToDstBuffer(size_t dataLength, void* initData, RHIResource* dstBuffer, size_t offset_dst)
	{
		auto bufferUploadCommand = [&](RHICmdList* curCmdList, RHIStagingBuffer* srcBuffer)
		{
			curCmdList->CopyBufferToBuffer(dstBuffer, 0, srcBuffer->mBufferData.get(), offset_dst, srcBuffer->mBufferSize);
		};
		UploadToDstResource(dataLength, initData, dstBuffer, offset_dst, bufferUploadCommand);
	}

	void RHIStagingBufferPool::UploadToDstTexture(size_t dataLength, void* initData, RHIResource* dstTexture, size_t offset_dst)
	{
		auto textureUploadCommand = [&](RHICmdList* curCmdList, RHIStagingBuffer* srcBuffer)
		{
			curCmdList->CopyBufferToTexture(dstTexture, 0, srcBuffer->mBufferData.get(), offset_dst);
		};
		UploadToDstResource(dataLength, initData, dstTexture, offset_dst, textureUploadCommand);
	}

	void RHIStagingBufferPool::TickPoolRefresh()
	{
		if (!isPoolWorking)
		{
			return;
		}
		size_t curFinishedValue = mCopyFence->GetCompletedValue();
		size_t curSubmmitValue = mCopyFence->GetNextSignalValue() - 1;
		if (curFinishedValue == curSubmmitValue)
		{
			//所有的拷贝任务都已经结束，可以清理掉
			isPoolWorking = false;
			mResourceCopying.clear();
			mCopyCommandList->Reset();
		}
	}

	RHIRenderQueuePtr GenerateRenderQueue(RHIQueueType queueType)
	{
		RHIRenderQueuePtr curRenderQueue;
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
			curRenderQueue = CreateRHIObject<DX12RenderQueue>(queueType);
		else
			curRenderQueue = CreateRHIObject<VulkanRenderQueue>(queueType);
		return curRenderQueue;
	}

	RHIDevicePtr GenerateRenderDevice()
	{
		RHIDevicePtr curDevice;
		if (Config_RenderDeviceType.GetValue() == "DirectX12")
			curDevice = CreateRHIObject<DX12Device>();
		else
			curDevice = CreateRHIObject<VulkanDevice>();
		return curDevice;
	}
}
