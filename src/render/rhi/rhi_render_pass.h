#pragma once

#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_descriptor.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/rhi_ptr.h"

#include <boost/container_hash/hash.hpp>

namespace luna::render
{

enum class RenderPassLoadOp
{
	kLoad = 0,
	kClear,
	kDontCare,
};

enum class RenderPassStoreOp
{
	kStore = 0,
	kDontCare,
};

struct PassColorDesc
{
	RHITextureFormat  mFormat = RHITextureFormat::FORMAT_UNKNOWN;
	RenderPassLoadOp  mLoadOp = RenderPassLoadOp::kClear;
	RenderPassStoreOp mStoreOp = RenderPassStoreOp::kStore;
	LVector4f         mClearColor = LVector4f(0, 0, 0, 1);

	auto MakeTie() const
	{
		return std::tie(mFormat, mLoadOp, mStoreOp);
	}
};

struct PassDepthStencilDesc
{
	RHITextureFormat  mDepthStencilFormat = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;

	RenderPassLoadOp  mDepthLoadOp = RenderPassLoadOp::kClear;
	RenderPassStoreOp mDepthStoreOp = RenderPassStoreOp::kStore;


	RenderPassLoadOp  mStencilLoadOp = RenderPassLoadOp::kDontCare;
	RenderPassStoreOp mStencilStoreOp = RenderPassStoreOp::kDontCare;

	float	     mClearDepth = 1;
	uint32_t	mClearStencil = 0;

	auto MakeTie() const
	{
		return std::tie(mDepthStencilFormat, mDepthStencilFormat, mDepthLoadOp, mDepthStoreOp, mStencilLoadOp, mStencilStoreOp);
	}
};

struct RenderPassDesc
{
	LVector<PassColorDesc>        mColors;
	LVector<RHIViewPtr>           mColorView;
	LVector<PassDepthStencilDesc> mDepths;
	RHIViewPtr                    mDepthStencilView;

	size_t Hash() const
	{
		size_t result = 0;
		boost::hash_combine(result, mColors.size());
		for (auto& view : mColorView)
		{
			boost::hash_combine(result, view->mBindResource->mResDesc.Format);
		}
		boost::hash_combine(result, mDepths.size());
		if (mDepthStencilView)
			boost::hash_combine(result, mDepthStencilView->mBindResource->mResDesc.Format);
		return result;
	}
};


class RENDER_API RHIRenderPass : public RHIObject
{
protected:
	RHISwapchainDesc m_render_window_desc;
	
	
public:
	RenderPassDesc mDesc;

};
}
