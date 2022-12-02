#include "render/rhi/vulkan/vulkan_frame_buffer.h"

#include "render/rhi/rhi_types.h"
#include "render/rhi/vulkan/vulkan_swapchain.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_render_pass.h"
#include "render/rhi/vulkan/vulkan_view.h"

namespace luna::render
{


VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferDesc& desc) :
	mWidth(desc.mWidth),
	mHeight(desc.mHeight)
{
	RHIView* colorRTV = desc.mColor[0];
	RHIView* dsv = desc.mDepthStencil;
	std::vector< VkImageView> attachments;
	if (colorRTV)
		attachments.push_back(colorRTV->As<VulkanView>()->mImageView);
	colorRTV->As<VulkanView>()->mColorAttachment = 0;
	if (dsv)
		attachments.push_back(dsv->As<VulkanView>()->mImageView);
	

	VkFramebufferCreateInfo framebufferInfo{};
	RHIRenderPass* vulkanPass = desc.mPass;
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = vulkanPass->As<VulkanRenderPass>()->mRenderPass;
	framebufferInfo.attachmentCount = (uint32_t)attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = mWidth;
	framebufferInfo.height = mHeight;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice(), &framebufferInfo, nullptr, &mBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create framebuffer!");
	}
}

}