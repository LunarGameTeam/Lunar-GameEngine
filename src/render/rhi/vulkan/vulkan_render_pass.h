#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/rhi_render_pass.h"

#include "render/render_module.h"

#include <vulkan/vulkan.hpp>

namespace luna::render
{



class RENDER_API VulkanRenderPass : public RHIRenderPass
{
public:
	VulkanRenderPass(const RenderPassDesc& desc);

	vk::RenderPass	mRenderPass;
};
}
