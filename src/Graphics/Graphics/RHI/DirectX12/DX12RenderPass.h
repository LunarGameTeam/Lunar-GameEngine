#pragma once
#include "Graphics/RenderConfig.h"
#include "DX12Pch.h"


namespace luna::graphics
{
	class RENDER_API Dx12RenderPass : public RHIRenderPass
	{
	public:
		RenderPassDesc	mRenderPassDesc;
		Dx12RenderPass(const RenderPassDesc& desc) : mRenderPassDesc(desc) {}
	};
}
