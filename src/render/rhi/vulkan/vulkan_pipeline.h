#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_device.h"

#include <vulkan/vulkan.hpp>


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
