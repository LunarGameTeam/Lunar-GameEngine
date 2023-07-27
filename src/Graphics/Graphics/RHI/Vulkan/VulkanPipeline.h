#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/Vulkan/VulkanPch.h"
#include "Graphics/RHI/RHIPipeline.h"

namespace luna::graphics
{

class RENDER_API VulkanPipelineState : public RHIPipelineState
{
public:
	VulkanPipelineState(const RHIPipelineStateDesc& pso_desc);
	void Init();


	vk::Pipeline mPipeline;	

};
}
