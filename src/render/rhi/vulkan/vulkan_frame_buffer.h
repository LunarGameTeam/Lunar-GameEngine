#pragma once
#include "render/render_config.h"
#include "render/rhi/rhi_frame_buffer.h"

#include "render/render_module.h"
#include <vulkan/vulkan.h>

namespace luna::render
{

class RENDER_API VulkanFrameBuffer : public RHIFrameBuffer
{
public:
	VulkanFrameBuffer(const FrameBufferDesc& desc);
	VkExtent2D GetExtent() { return VkExtent2D(mWidth, mHeight); }

	RHIViewPtr mRtv;
	RHIViewPtr mDsv;
	uint32_t         mWidth;
	uint32_t         mHeight;
	VkFramebuffer    mBuffer;

};
}
