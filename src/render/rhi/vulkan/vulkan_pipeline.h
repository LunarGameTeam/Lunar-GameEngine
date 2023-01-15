#pragma once
#include "render/render_config.h"
#include "render/rhi/rhi_device.h"
#include "render/rhi/vulkan/vulkan_pch.h"
#include "render/rhi/rhi_pipeline.h"

namespace luna::render
{

class RENDER_API VulkanPipelineState : public RHIPipelineState
{
public:
	VulkanPipelineState(const RHIPipelineStateDesc& pso_desc);
	void Init();


	vk::Pipeline mPipeline;	

};
}
