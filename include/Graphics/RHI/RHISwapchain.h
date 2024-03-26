#pragma once

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Core/Platform/Window.h"



namespace luna::graphics
{

class RHIRenderQueue;

class RENDER_API RHISwapChain : public RHIObject
{
public:
	RHISwapchainDesc           mWindowDesc;
	LArray<RHIResourcePtr>    mBackBuffers;
	LArray<RHIViewPtr>        mViews;
	LWindow*                   mWindow = nullptr;
protected:
	RHIRenderQueue*            mQueue = nullptr;
public:
	RHISwapChain(LWindow* window, const RHISwapchainDesc& windowDesc, RHIRenderQueue* queue) :
		mWindowDesc(windowDesc),
		mWindow(window),
		mQueue(queue)
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
