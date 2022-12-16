#pragma once
#include "render/render_config.h"
#include "render/rhi/vulkan/vulkan_pch.h"
#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/rhi_render_pass.h"

#include "render/render_module.h"


namespace luna::render
{



class RENDER_API VulkanRenderPass : public RHIRenderPass
{
public:
	VulkanRenderPass(const RenderPassDesc& desc);

	vk::RenderPass	mRenderPass;
};
}
