#pragma once

#include "Core/Math/Math.h"

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/RHIPtr.h"


namespace luna::render
{

enum class LoadOp
{
	kLoad = 0,
	kClear,
	kDontCare,
};

enum class StoreOp
{
	kStore = 0,
	kDontCare,
};

struct RENDER_API PassColorDesc
{
	LoadOp    mLoadOp = LoadOp::kClear;
	StoreOp   mStoreOp = StoreOp::kStore;
	LVector4f mClearColor = LVector4f(0, 0, 0, 1);

};

struct RENDER_API PassDepthStencilDesc
{
	RHITextureFormat  mDepthStencilFormat = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;

	LoadOp  mDepthLoadOp = LoadOp::kClear;
	StoreOp mDepthStoreOp = StoreOp::kStore;


	LoadOp  mStencilLoadOp = LoadOp::kDontCare;
	StoreOp mStencilStoreOp = StoreOp::kDontCare;

	float	     mClearDepth = 1;
	uint32_t	mClearStencil = 0;

	auto MakeTie() const
	{
		return std::tie(mDepthStencilFormat, mDepthStencilFormat, mDepthLoadOp, mDepthStoreOp, mStencilLoadOp, mStencilStoreOp);
	}
};

struct RENDER_API RenderPassDesc
{
	LArray<PassColorDesc>        mColors;
	LArray<RHIViewPtr>           mColorView;
	LArray<PassDepthStencilDesc> mDepths;
	RHIViewPtr                   mDepthStencilView;

	size_t Hash() const;
};


class RENDER_API RHIRenderPass : public RHIObject
{
protected:
	RHISwapchainDesc m_render_window_desc;
	
	
public:
	RenderPassDesc mDesc;

};
}
