#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/Vulkan/VulkanPch.h"
#include "Graphics/RHI/RHIPipeline.h"

namespace luna::graphics
{

class RENDER_API VulkanPipelineStateGraphic : public RHIPipelineStateGraphic
{
public:
	vk::Pipeline mPipeline;
	VulkanPipelineStateGraphic(
		LSharedPtr<RHIPipelineStateDescBase> psoDesc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	);

private:
	void CreateGraphDrawPipelineImpl(
		RHIPipelineStateGraphDrawDesc* graphPipelineDesc,
		RHIVertexLayout& inputLayout,
		RenderPassDesc& renderPassDesc
	) override;

};

class RENDER_API VulkanPipelineStateCompute : public RHIPipelineStateCompute
{
public:
	vk::Pipeline mPipeline;
	VulkanPipelineStateCompute(
		LSharedPtr<RHIPipelineStateDescBase> psoDesc
	);

private:
	void CreateComputePipelineImpl(
		RHIPipelineStateComputeDesc* computePipelineDesc
	) override;
};
}
