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
		mCopyCommandList = mDevice->CreateCommondList(RHICmdListType::Copy);
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

	void RHIStagingBufferPool::UploadToDstBuffer(RHIStagingBuffer* srcBuffer, size_t offset_src, RHIResource* dstBuffer, size_t offset_dst)
	{
		mCopyCommandList->Reset();
		if (dstBuffer->GetInitialState() != kCopyDest)
		{
			ResourceBarrierDesc dstBarrier;
			dstBarrier.mBarrierRes = dstBuffer;
			dstBarrier.mStateBefore = dstBuffer->GetInitialState();
			dstBarrier.mStateAfter = kCopyDest;
			mCopyCommandList->ResourceBarrierExt(dstBarrier);
		}
		
		mCopyCommandList->CopyBufferToBuffer(dstBuffer, 0, srcBuffer->mBufferData.get(), 0, srcBuffer->mBufferSize);

		ResourceBarrierDesc undoBarrier;
		undoBarrier.mBarrierRes = dstBuffer;
		undoBarrier.mStateBefore = kCopyDest;
		undoBarrier.mStateAfter = kCommon;
		mCopyCommandList->ResourceBarrierExt(undoBarrier);
		mCopyCommandList->CloseCommondList();
		mRenderQueue->ExecuteCommandLists(mCopyCommandList.get());
		//记录当前buffer需要监控的拷贝指令fence
		size_t nextSignalValue = mCopyFence->GetNextSignalValue();
		mResourceCopying[nextSignalValue].push_back(mResourceWaitingCopy[srcBuffer->mResourceId]);
		mResourceWaitingCopy.erase(srcBuffer->mResourceId);
		mUnusedResourceId.push(srcBuffer->mResourceId);
		//插入一个桩
		mCopyFence->IncSignal(mRenderQueue);
		isPoolWorking = true;
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
	}
}
