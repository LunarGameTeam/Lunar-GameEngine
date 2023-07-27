#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/Vulkan/VulkanPch.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/RHIRenderPass.h"

#include "Graphics/RenderModule.h"


namespace luna::graphics
{



class RENDER_API VulkanRenderPass : public RHIRenderPass
{
public:
	VulkanRenderPass(const RenderPassDesc& desc);

	vk::RenderPass	mRenderPass;
};
}
