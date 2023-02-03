#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIFrameBuffer.h"

#include "Graphics/RenderModule.h"
#include <vulkan/vulkan.hpp>

namespace luna::render
{

class RENDER_API VulkanFrameBuffer : public RHIFrameBuffer
{
public:
	VulkanFrameBuffer(const FrameBufferDesc& desc);
	virtual ~VulkanFrameBuffer();
	VkExtent2D GetExtent() { return VkExtent2D(mWidth, mHeight); }

	RHIViewPtr mRtv;
	RHIViewPtr mDsv;
	uint32_t         mWidth;
	uint32_t         mHeight;
	vk::Framebuffer    mBuffer;

};
}
