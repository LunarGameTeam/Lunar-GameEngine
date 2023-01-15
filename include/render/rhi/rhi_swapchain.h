#pragma once

#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/rhi_fence.h"
#include "render/rhi/rhi_ptr.h"
#include "core/platform/window.h"



namespace luna::render
{


class RENDER_API RHISwapChain : public RHIObject
{
public:
	RHISwapchainDesc           mWindowDesc;
	LArray<RHIResourcePtr>    mBackBuffers;
	LArray<RHIViewPtr>        mViews;
	LWindow*                   mWindow = nullptr;

public:
	RHISwapChain(LWindow* window, const RHISwapchainDesc& windowDesc) :
		mWindowDesc(windowDesc),
		mWindow(window)
	{
	}

	inline const RHISwapchainDesc& GetDesc()
	{
		return mWindowDesc;
	}
	inline uint32_t GetWidth()
	{
		return mWindowDesc.mWidth;
	}
	inline uint32_t GetHeight()
	{
		return mWindowDesc.mHeight;
	}
	RHIResourcePtr GetBackBuffer(const uint32_t idx)
	{
		return mBackBuffers[idx];
	}

	virtual void PresentFrame(RHIFence* fence, uint64_t waitValue)     = 0;
	virtual uint32_t GetNowFrameID()                                   = 0;
	virtual uint32_t NextImage() = 0;
	virtual bool Reset(const RHISwapchainDesc& window_width_in)        = 0;
};
}
