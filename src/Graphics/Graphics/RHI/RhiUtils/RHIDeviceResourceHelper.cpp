#pragma once

#include "Graphics/RHI/RhiUtils/RHIDeviceResourceHelper.h"
#include "Graphics/RHI/RHIDevice.h"
namespace luna::graphics
{
	DeviceResourceGenerateHelper::DeviceResourceGenerateHelper()
	{
		mStagingBufferPool = MakeShared<RHIStagingBufferPool>();
		mBarrierCmd = sGlobelRenderDevice->CreateSinglePoolSingleCommondList(RHICmdListType::Graphic3D);
	}

	RHIResourcePtr DeviceResourceGenerateHelper::CreateBuffer(RHIHeapType memoryType, RHIBufferDesc& resDesc, void* initData, size_t initDataSize)
	{
		assert(resDesc.mSize != 0);
		resDesc.mBufferUsage = resDesc.mBufferUsage | RHIBufferUsage::TransferDstBit;
		RHIResourcePtr dstBuffer = sGlobelRenderDevice->CreateBufferExt(resDesc);
		dstBuffer->BindMemory(memoryType);
		if (initData == nullptr)
		{
			return dstBuffer;
		}
		assert(initDataSize != 0);
		if (memoryType == RHIHeapType::Upload)
		{
			void* dataPointer = dstBuffer->Map();
			memcpy(dataPointer, initData, initDataSize);
			dstBuffer->Unmap();
		}
		else
		{
			mStagingBufferPool->UploadToDstBuffer(initDataSize, initData, dstBuffer, 0);
		}
		return dstBuffer;
	}

	RHIResourcePtr DeviceResourceGenerateHelper::CreateTexture(RHIResDesc resDesc, void* initData, size_t dataSize, const RHISubResourceCopyDesc sourceCopyOffset)
	{
		bool usingStaging = false;
		usingStaging = (initData != nullptr);

		RHIResourcePtr stagingBuffer = nullptr;

		RHIResDesc newResDesc = resDesc;
		newResDesc.mImageUsage = newResDesc.mImageUsage | RHIImageUsage::TransferDstBit;
		RHIResourcePtr textureRes = sGlobelRenderDevice->CreateTextureExt(newResDesc);
		const MemoryRequirements& memoryReq = textureRes->GetMemoryRequirements();
		textureRes->BindMemory(RHIHeapType::Default);
		if (initData == nullptr)
		{
			return textureRes;
		}
		assert(dataSize != 0);
		mStagingBufferPool->UploadToDstTexture(dataSize, initData, sourceCopyOffset, textureRes, 0);

		ResourceBarrierDesc dstBarrier;
		dstBarrier.mBarrierRes = textureRes;
		dstBarrier.mBaseMipLevel = 0;
		dstBarrier.mMipLevels = resDesc.MipLevels;
		dstBarrier.mBaseDepth = 0;
		dstBarrier.mDepth = resDesc.DepthOrArraySize;
		if (Has(resDesc.mImageUsage, RHIImageUsage::SampledBit))
		{
			dstBarrier.mStateBefore = kCommon;
			dstBarrier.mStateAfter = kShaderReadOnly;
			mAllResGenerateBarrier.push_back(dstBarrier);
		}
		else if (Has(resDesc.mImageUsage, RHIImageUsage::ColorAttachmentBit))
		{
			dstBarrier.mStateBefore = kCommon;
			dstBarrier.mStateAfter = kRenderTarget;
			mAllResGenerateBarrier.push_back(dstBarrier);
			mAllResGenerateBarrier.push_back(dstBarrier);
		}
		return textureRes;
	}

	RHICmdList* DeviceResourceGenerateHelper::FlushStaging()
	{
		if (mAllResGenerateBarrier.size() == 0)
		{
			return nullptr;
		}
		mBarrierCmd->Reset();
		mBarrierCmd->GetCmdList()->ResourceBarrierExt(mAllResGenerateBarrier);
		mBarrierCmd->GetCmdList()->CloseCommondList();
		mAllResGenerateBarrier.clear();
		return mBarrierCmd->GetCmdList();
		//sRenderModule->GetRenderContext()->mGraphicQueue->ExecuteCommandLists(mBarrierCmd->GetCmdList());
	}

	void DeviceResourceGenerateHelper::CheckStageRefresh()
	{
		mStagingBufferPool->TickPoolRefresh();
	}

	RHIResourcePtr DeviceResourceGenerateHelper::CreateBufferByRhiMemory(const RHIBufferDesc& desc, RHIMemoryPtr targetMemory, size_t& memoryOffset)
	{
		assert(desc.mSize != 0);
		RHIBufferDesc resDesc = desc;
		resDesc.mBufferUsage = resDesc.mBufferUsage | RHIBufferUsage::TransferDstBit;
		RHIResourcePtr dstBuffer = sGlobelRenderDevice->CreateBufferExt(resDesc);
		dstBuffer->BindMemory(targetMemory, memoryOffset);
		return dstBuffer;
	}

	RHIResourcePtr DeviceResourceGenerateHelper::CreateTextureByRhiMemory(const RHIResDesc& resDesc, RHIMemoryPtr targetMemory, size_t& memoryOffset)
	{
		RHIResDesc newResDesc = resDesc;
		newResDesc.mImageUsage = newResDesc.mImageUsage | RHIImageUsage::TransferDstBit;
		RHIResourcePtr textureRes = sGlobelRenderDevice->CreateTextureExt(newResDesc);
		const MemoryRequirements& memoryReq = textureRes->GetMemoryRequirements();
		textureRes->BindMemory(targetMemory, memoryOffset);

		return textureRes;
	}

}
