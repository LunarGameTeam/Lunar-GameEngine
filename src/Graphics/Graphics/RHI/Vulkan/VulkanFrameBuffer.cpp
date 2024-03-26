#include "Graphics/RHI/Vulkan/VulkanFrameBuffer.h"

#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanRenderPass.h"
#include "Graphics/RHI/Vulkan/VulkanView.h"

namespace luna::graphics
{


VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferDesc& desc) :
	mWidth(desc.mWidth),
	mHeight(desc.mHeight)
{
	RHIView* colorRTV = desc.mColor[0];
	RHIView* dsv = desc.mDepthStencil;
	std::vector<vk::ImageView> attachments;
	if (colorRTV)
		attachments.push_back(colorRTV->As<VulkanView>()->mImageView);
	colorRTV->As<VulkanView>()->mColorAttachment = 0;
	if (dsv)
		attachments.push_back(dsv->As<VulkanView>()->mImageView);
	

	vk::FramebufferCreateInfo framebufferInfo{};
	RHIRenderPass* vulkanPass = desc.mPass;	
	framebufferInfo.renderPass = vulkanPass->As<VulkanRenderPass>()->mRenderPass;
	framebufferInfo.attachmentCount = (uint32_t)attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = mWidth;
	framebufferInfo.height = mHeight;
	framebufferInfo.layers = 1;

	sGlobelRenderDevice->As<VulkanDevice>()->GetVKDevice().createFramebuffer(&framebufferInfo, nullptr, &mBuffer);
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	vk::Device device = sGlobelRenderDevice->As<VulkanDevice>()->GetVKDevice();
	if (mBuffer)
		vkDestroyFramebuffer(device, mBuffer, nullptr);
}

}