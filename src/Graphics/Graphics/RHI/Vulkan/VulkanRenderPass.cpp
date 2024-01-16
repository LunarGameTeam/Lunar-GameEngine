#include "Graphics/RHI/Vulkan/VulkanRenderPass.h"

#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"
#include "Graphics/RHI/Vulkan/VulkanFrameBuffer.h"

#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanResource.h"

namespace luna::graphics
{


VulkanRenderPass::VulkanRenderPass(const RenderPassDesc& desc)
{
	mDesc = desc;

	std::vector<vk::SubpassDescription> subpasses;
	std::vector<vk::AttachmentDescription> descriptions;
	
	vk::Device mDevice = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	auto convertLoad = [](LoadOp loadOp)->auto
	{
		switch (loadOp)
		{
		case LoadOp::kLoad:
			return vk::AttachmentLoadOp::eLoad;
		case LoadOp::kClear:
			return vk::AttachmentLoadOp::eClear;
		case LoadOp::kDontCare:
			return vk::AttachmentLoadOp::eDontCare;
		}
		return vk::AttachmentLoadOp::eNoneEXT;
		assert(false);
	};
	auto convertStore = [](StoreOp loadOp)->auto
	{
		switch (loadOp)
		{
		case StoreOp::kStore:
			return vk::AttachmentStoreOp::eStore;
		case StoreOp::kDontCare:
			return vk::AttachmentStoreOp::eDontCare;
		}
		assert(false);
		return vk::AttachmentStoreOp::eNone;		
	};
	for (int i = 0; i < desc.mColors.size(); ++i)
	{
		const PassColorDesc& it = desc.mColors[i];
		RHIViewPtr view = desc.mColorView[i];
		vk::AttachmentDescription& colorAttachment = descriptions.emplace_back();
		colorAttachment.format = Convert(view->mBindResource->GetDesc().Format);
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = convertLoad(it.mLoadOp);
		colorAttachment.storeOp = convertStore(it.mStoreOp);
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = (uint32_t)descriptions.size() - 1;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription& subpass = subpasses.emplace_back();
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
	}

	if (!desc.mDepths.empty())
	{
		vk::AttachmentDescription& depthAttachment = descriptions.emplace_back();
		depthAttachment.format = Convert(desc.mDepths[0].mDepthStencilFormat);
		depthAttachment.samples = vk::SampleCountFlagBits::e1;
		depthAttachment.loadOp = convertLoad(desc.mDepths[0].mDepthLoadOp);
		depthAttachment.storeOp = convertStore(desc.mDepths[0].mDepthStoreOp);
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eLoad;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = (uint32_t)descriptions.size() - 1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
		
		subpasses[0].pDepthStencilAttachment = &depthAttachmentRef;
	}


	vk::RenderPassCreateInfo renderPassInfo{};
	renderPassInfo.attachmentCount = (uint32_t)descriptions.size();
	renderPassInfo.pAttachments = descriptions.data();
	renderPassInfo.subpassCount = (uint32_t)subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	
	VULKAN_ASSERT(mDevice.createRenderPass(&renderPassInfo, nullptr, &mRenderPass));
}

}