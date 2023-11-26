#pragma once

#include "Graphics/RHI/RHIResource.h"
namespace luna::graphics
{
	RHIResource::RHIResource()
	{

	}
	RHIResource::RHIResource(const RHIResDesc& desc) :
		mResDesc(desc)
	{

	}

	RHIResource::RHIResource(const RHIBufferDesc& desc)
	{
		mResDesc.mType = ResourceType::kBuffer;
		mResDesc.Dimension = RHIResDimension::Buffer;
		mResDesc.Width = desc.mSize;
		mResDesc.mUsage = RHIResourceUsage::RESOURCE_FLAG_NONE;
	}

	RHIResource::~RHIResource()
	{

	};

	void RHIResource::ResetResourceBufferSize(size_t newSize)
	{
		if (mResDesc.Width != newSize)
		{
			mSizeDirty = true;
			mResDesc.Width = newSize;
			ResetResourceBufferSizeDeviceData(newSize);
		}
	}

	void RHIResource::SetInitialState(ResourceState state)
	{
		mState = state;
	};

	ResourceState RHIResource::GetInitialState() const
	{
		return mState;
	};

	const RHIResDesc& RHIResource::GetDesc() const
	{
		return mResDesc;
	}

	const MemoryRequirements& RHIResource::GetMemoryRequirements() const
	{
		if (mSizeDirty)
		{
			RefreshMemoryRequirements();
			mSizeDirty = false;
		}
		return mMemoryLayout;
	}
}
