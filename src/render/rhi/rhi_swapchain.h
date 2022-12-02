#pragma once

#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/rhi_fence.h"
#include "render/rhi/rhi_ptr.h"
#include "window/window.h"



namespace luna::render
{

class RENDER_API RHISwapChain : public RHIObject
{
public:
	RHIWindowDesc           mWindowDesc;
	LVector<RHIResourcePtr> mBackBuffers;	
	LWindow*                mWindow = nullptr;

public:
	RHISwapChain(LWindow* window, const RHIWindowDesc& windowDesc) :
		mWindowDesc(windowDesc),
		mWindow(window)
	{
	}

	inline const RHIWindowDesc& GetDesc()
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

	virtual void PresentFrame(RHIFence* fence, uint64_t waitValue) = 0;
	virtual uint32_t GetNowFrameID() = 0;
	virtual uint32_t NextImage(RHIFence* fence, uint64_t signal_value) = 0;
	virtual bool Reset(const RHIWindowDesc& window_width_in) = 0;
};
}
