#pragma once
#include "render/pch.h"
#include "dx12_rhi.h"


namespace luna::render
{
	class RENDER_API Dx12RenderPass : public RHIRenderPass
	{
	public:
		RenderPassDesc	mRenderPassDesc;
		Dx12RenderPass(const RenderPassDesc& desc) : mRenderPassDesc(desc) {}
	};
}
