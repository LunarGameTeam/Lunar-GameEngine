#pragma once

#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RhiUtils/RHIUniformBuffer.h"
namespace luna::graphics
{
	RhiUniformBufferPack::~RhiUniformBufferPack()
	{
		if (mPool != nullptr)
		{
			mPool->FreeUniform(this);
		}
	}

	RhiUniformBufferPack::RhiUniformBufferPack()
	{
		mPool = nullptr;
		mPhysicBuffer = nullptr;
		mOffset = 0;
		mSize = 0;
	}

	RhiUniformBufferPack::RhiUniformBufferPack(
		RhiUniformBufferPool* pool,
		RHIResource* physicBuffer,
		size_t offset,
		size_t size,
		size_t uid
	) :mPool(pool)
	{
		mPhysicBuffer = physicBuffer;
		mOffset = mOffset;
		mSize = size;
		mUid = uid;
	}

	RhiUniformBufferPool::RhiUniformBufferPool(DeviceResourceGenerateHelper* resCreateHelper)
	{
		mResCreateHelper = resCreateHelper;
		mSizePerBuffer = luna::CommonSize256K;
		mAllocOffset = 0;
	}

	RhiUniformBufferPack RhiUniformBufferPool::AllocUniform(size_t uniformSize)
	{
		size_t alignedAllocSize = luna::SizeAligned2Pow(uniformSize, 256);
		auto poolArrayItor = mEmptyPool.find(alignedAllocSize);
		if (poolArrayItor != mEmptyPool.end() && !poolArrayItor->second.empty())
		{
			auto Itor = poolArrayItor->second.begin();
			RhiUniformBufferPack curPack = Itor->second;
			poolArrayItor->second.erase(Itor);
			return curPack;
		}
		if (!CheckHasEnoughSize(alignedAllocSize))
		{
			GenerateNewBuffer();
		}
		RhiUniformBufferPack curPack(this, resourcePack.back().get(), mAllocOffset, alignedAllocSize, mMaxUid);
		mMaxUid += 1;
		mAllocOffset += alignedAllocSize;
		return curPack;
	}

	void RhiUniformBufferPool::CreateUniformBufferAndView(const RHICBufferDesc& uniform, RhiUniformBufferPack& bufferOut, RHIViewPtr& viewOut)
	{
		bufferOut = AllocUniform(uniform.mSize);
		ViewDesc paramBufferviewDesc;
		paramBufferviewDesc.mViewType = RHIViewType::kConstantBuffer;
		paramBufferviewDesc.mViewDimension = RHIViewDimension::BufferView;
		paramBufferviewDesc.mOffset = bufferOut.GetOffset();
		viewOut = sGlobelRenderDevice->CreateView(paramBufferviewDesc);
		viewOut->BindResource(bufferOut.GetRhiBuffer());
	}

	bool RhiUniformBufferPool::CheckHasEnoughSize(size_t sizeAlloc)
	{
		return mSizePerBuffer - mAllocOffset - 256 > sizeAlloc;
	}

	void RhiUniformBufferPool::GenerateNewBuffer()
	{
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
		desc.mSize = mSizePerBuffer;
		RHIResourcePtr newCbuffer = mResCreateHelper->CreateBuffer(RHIHeapType::Default, desc);
		resourcePack.push_back(newCbuffer);
		mAllocOffset = 0;
	}

	void RhiUniformBufferPool::FreeUniform(RhiUniformBufferPack* uniformData)
	{
		LUnorderedMap<size_t, RhiUniformBufferPack>& curPool = mEmptyPool[uniformData->GetSize()];
		auto itor = curPool.find(uniformData->GetUid());
		if (itor != curPool.end())
		{
			return;
		}
		RhiUniformBufferPack newPack = *uniformData;
		curPool.insert({ uniformData->GetUid(),newPack });
	}
}
