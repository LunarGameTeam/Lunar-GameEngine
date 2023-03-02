#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIDevice.h"


#include "Graphics/RHI/Vulkan/VulkanCmdList.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanFrameBuffer.h"
#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"
#include "Graphics/RHI/Vulkan/VulkanBindingSetLayout.h"
#include "Graphics/RHI/Vulkan/VulkanPipeline.h"
#include "Graphics/RHI/Vulkan/VulkanRenderPass.h"
#include "Graphics/RHI/Vulkan/VulkanResource.h"
#include "Graphics/RHI/Vulkan/VulkanBindingSet.h"
#include "Graphics/RHI/Vulkan/VulkanView.h"

#include "Graphics/RenderModule.h"

namespace luna::render
{

VulkanGraphicCmdList::VulkanGraphicCmdList(RHICmdListType type) : 
	RHIGraphicCmdList(type)
{
	mClosed = true;
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies();
	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	if (type == RHICmdListType::Graphic3D)
	{
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	}
	else if (type == RHICmdListType::Copy)
	{
		poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
	}


	VULKAN_ASSERT(sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().createCommandPool(&
		poolInfo, nullptr, &mCommandPool));
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.commandPool = mCommandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;
	
	VULKAN_ASSERT(sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().allocateCommandBuffers(&allocInfo, &mCommandBuffer));

}

void VulkanGraphicCmdList::BeginEvent(const LString& event_str)
{
	if (sRenderModule->GetDevice<VulkanDevice>()->mEnableValidation)
	{
		vk::DebugUtilsLabelEXT label = {};
		label.pLabelName = event_str.c_str();
		mCommandBuffer.beginDebugUtilsLabelEXT(&label, sRenderModule->GetDevice<VulkanDevice>()->GetLoader());
	}
}

void VulkanGraphicCmdList::EndEvent()
{
	if (sRenderModule->GetDevice<VulkanDevice>()->mEnableValidation)
	{
		mCommandBuffer.endDebugUtilsLabelEXT(sRenderModule->GetDevice<VulkanDevice>()->GetLoader());
	}
}

void VulkanGraphicCmdList::CloseCommondList()
{
	if (!mClosed)
	{
		mCommandBuffer.end();		
		mClosed = true;
	}
	
}

void VulkanGraphicCmdList::ClearDSView(uint32_t sr_left, uint32_t sr_top, uint32_t sr_right, uint32_t sr_bottom, RHIView* descriptor_dsv, float depth, uint8_t stencil)
{

}

void VulkanGraphicCmdList::ClearRTView(RHIView* descriptor_rtv, LVector4f clear_color,
	int x /*= 0*/, int y /*= 0*/, int width /*= 0*/, int height /*= 0*/)
{
	// Visible pass
	// Clear color and depth attachments
	VulkanView* rtv = descriptor_rtv->As<VulkanView>();
	vk::ClearAttachment clearAttachments[1] = {};

	if (width == 0)
		width = rtv->mRes->GetDesc().Width;
	if (height == 0)
		height = rtv->mRes->GetDesc().Height;

	vk::ClearColorValue vkClearColor;
	vkClearColor.setFloat32({ clear_color.x(), clear_color.y(), clear_color.z(), clear_color.w() });
	clearAttachments[0].aspectMask = vk::ImageAspectFlagBits::eColor;
	clearAttachments[0].clearValue.color = vkClearColor;
	clearAttachments[0].colorAttachment = descriptor_rtv->As<VulkanView>()->mColorAttachment;

	vk::ClearRect clearRect = {};
	clearRect.layerCount = 1;
	clearRect.rect.setOffset({ x, y });
	clearRect.rect.setExtent({(uint32_t)width, (uint32_t)height});
	mCommandBuffer.clearAttachments(
		1,
		clearAttachments,
		1,
		&clearRect
	);
}

void VulkanGraphicCmdList::SetScissorRects(size_t x, size_t y, size_t width, size_t height)
{
	vk::Rect2D scissor{};
	scissor.offset = vk::Offset2D(0, 0);
	scissor.extent = vk::Extent2D((uint32_t)width  , (uint32_t)height );
	mCommandBuffer.setScissor(0, 1, &scissor);
}

void VulkanGraphicCmdList::DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StartIndexLocation, int32_t BaseVertexLocation, int32_t StartInstanceLocation)
{
	mCommandBuffer.drawIndexed(IndexCountPerInstance, 1, 0, 0, StartInstanceLocation);
}

void VulkanGraphicCmdList::DrawIndirectCommands(const RHICmdArgBuffer* DrawBuffer)
{
	size_t perDrawCommand = DrawBuffer->GetCmdSignature()->GetDesc().size();
	size_t allDrawCount = DrawBuffer->GetAllCommand().size() / perDrawCommand;
	for (int i = 0; i < allDrawCount; ++i)
	{
		int commandOffset = i * perDrawCommand;
		for (int j = 0; j < perDrawCommand; ++j)
		{
			const RHICmdArgBufferDataDesc& command = DrawBuffer->GetAllCommand()[commandOffset + j];
			switch (command.mArgType)
			{
			case RHIIndirectArgumentType::AsVertexBuffer:
			{
				SetVertexBuffer(command.mVbPointer, DrawBuffer->GetCmdSignature()->GetDesc()[j].mSlotOrIndex);
				break;
			}
			case RHIIndirectArgumentType::AsIndexBuffer:
			{
				SetIndexBuffer(command.mIbPointer);
				break;
			}
			case RHIIndirectArgumentType::AsDrawCommand:
			{
				DrawIndexedInstanced(command.mIndexCountPerInstance, command.mInstanceCount, command.mStartIndexLocation, command.mBaseVertexLocation, command.mStartInstanceLocation);
			}
			default:
				break;
			}
		}
	}
}

void VulkanGraphicCmdList::SetDrawPrimitiveTopology(RHIPrimitiveTopology primitive_topology)
{
	mCommandBuffer.setPrimitiveTopology(Convert(primitive_topology));
}

void VulkanGraphicCmdList::ResourceBarrierExt(const ResourceBarrierDesc& barrier)
{
	if (!barrier.mBarrierRes)
	{
		assert(false);
		return;
	}
	barrier.mBarrierRes->mState = barrier.mStateAfter;
	VulkanResource* vkRes = barrier.mBarrierRes->As<VulkanResource>();
	if (vkRes->GetDesc().mType == ResourceType::kBuffer)
	{
		VkBufferMemoryBarrier memoryBarrier = {};
		memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		VkBuffer buffer = vkRes->mBuffer;
		memoryBarrier.size = vkRes->GetMemoryRequirements().size;
		memoryBarrier.buffer = buffer;
		memoryBarrier.srcAccessMask = VkAccessFlagBits::VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR
			| VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR
			| VK_ACCESS_SHADER_READ_BIT
			| VK_ACCESS_SHADER_WRITE_BIT;
		memoryBarrier.dstAccessMask = memoryBarrier.srcAccessMask;		
		vkCmdPipelineBarrier(mCommandBuffer, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkDependencyFlagBits::VK_DEPENDENCY_BY_REGION_BIT,
			0, nullptr,
			1, &memoryBarrier,			
			0, nullptr);
		return;
	}
	else if (vkRes->GetDesc().mType == ResourceType::kTexture)
	{

		vk::ImageMemoryBarrier imageBarrier = {};
		VkImage image = vkRes->mImage;

		vk::ImageLayout vk_state_before = ConvertState(barrier.mStateBefore);
		vk::ImageLayout vk_state_after = ConvertState(barrier.mStateAfter);
		if (vk_state_before == vk_state_after)
			return;

		imageBarrier.oldLayout = vk_state_before;
		imageBarrier.newLayout = vk_state_after;
		imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.image = image;
		
		vk::ImageSubresourceRange& range = imageBarrier.subresourceRange;

		if (Has(barrier.mBarrierRes->GetDesc().mImageUsage, RHIImageUsage::DepthStencilBit))
		{

			range.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		}
		else
		{
			range.aspectMask = vk::ImageAspectFlagBits::eColor;

		}
		range.baseMipLevel = barrier.mBaseMipLevel;
		range.levelCount = barrier.mMipLevels;
		range.baseArrayLayer = barrier.mBaseDepth;
		range.layerCount = barrier.mDepth;

		// Source layouts (old)
		// Source access mask controls actions that have to be finished on the old layout
		// before it will be transitioned to the new layout
		switch (imageBarrier.oldLayout)
		{
		case vk::ImageLayout::eUndefined:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eNone;
			break;
		case vk::ImageLayout::ePreinitialized:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
			break;
		case vk::ImageLayout::eColorAttachmentOptimal:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			break;
		case vk::ImageLayout::eDepthAttachmentOptimal:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			break;
		case vk::ImageLayout::eTransferSrcOptimal:
			// Image is a transfer source 
			// Make sure any reads from the image have been finished
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			break;
		case vk::ImageLayout::eTransferDstOptimal:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			break;

		case vk::ImageLayout::eShaderReadOnlyOptimal:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
			break;
		case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
			imageBarrier.srcAccessMask = vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		// Target layouts (new)
		// Destination access mask controls the dependency for the new image layout
		switch (imageBarrier.newLayout)
		{
		case vk::ImageLayout::eTransferDstOptimal:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			imageBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			break;

		case vk::ImageLayout::eTransferSrcOptimal:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			imageBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
			break;

		case vk::ImageLayout::eColorAttachmentOptimal:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			imageBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
			break;

		case vk::ImageLayout::eDepthAttachmentOptimal:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			imageBarrier.dstAccessMask = imageBarrier.dstAccessMask | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			break;

		case vk::ImageLayout::eShaderReadOnlyOptimal:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (!imageBarrier.srcAccessMask)
			{
				imageBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
			}
			imageBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			break;
		case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
			imageBarrier.dstAccessMask = vk::AccessFlagBits::eFragmentShadingRateAttachmentReadKHR;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}		
		mCommandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eAllCommands,
			vk::DependencyFlagBits::eByRegion, 0, nullptr,
			0, nullptr,
			1, &imageBarrier
		);
		return;
	}
	assert(false);
}

void VulkanGraphicCmdList::SetBiningSetLayoutExt(RHIBindingSetLayout* root_signature)
{

}

void VulkanGraphicCmdList::BindDescriptorHeap()
{

}

void VulkanGraphicCmdList::SetVertexBuffer(const LArray<RHIVertexBufferDesc>& vb, int32_t begin_slot)
{
	std::vector<vk::Buffer> buffer;
	std::vector<vk::DeviceSize> offsets;
	std::vector<vk::DeviceSize> sizes;
	std::vector<vk::DeviceSize> strides;
	buffer.reserve(vb.size());
	for (const RHIVertexBufferDesc& it : vb)
	{
		buffer.push_back(it.mVertexRes->As<VulkanResource>()->mBuffer);
		offsets.push_back(it.mOffset);
		sizes.push_back(it.mBufferSize);
		strides.push_back(it.mVertexStride);
	}	
	mCommandBuffer.bindVertexBuffers2(0, buffer, offsets, sizes, strides);	
}

void VulkanGraphicCmdList::SetIndexBuffer(RHIResource* indexRes)
{
	vkCmdBindIndexBuffer(mCommandBuffer, indexRes->As<VulkanResource>()->mBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void VulkanGraphicCmdList::CopyBufferToBuffer(RHIResource* dstRes, uint32_t offset, RHIResource* srcRes, uint32_t srcOffset, size_t size)
{
	VulkanResource* vkDstRes = dstRes->As<VulkanResource>();
	VulkanResource* vkSrcRes = srcRes->As<VulkanResource>();
	VkBufferCopy vkRegions = {
		srcOffset, offset, size
	};
	vkCmdCopyBuffer(mCommandBuffer, vkSrcRes->mBuffer, vkDstRes->mBuffer, 1, &vkRegions);
}

void VulkanGraphicCmdList::CopyBufferToTexture(RHIResource* dst, uint32_t target_subresource, RHIResource* src, uint32_t source_subresource)
{
	VulkanResource* vkRes = dst->As<VulkanResource>();
	VulkanResource* vkSrcRes = src->As<VulkanResource>();

	std::vector<VkBufferImageCopy> bufferCopyRegions;

	size_t offset = 0;

	size_t totalBytes;
	size_t rowBytes;
	size_t rows;
	GetFormatInfo(vkRes->GetDesc().Width, vkRes->GetDesc().Height, dst->GetDesc().Format, totalBytes, rowBytes, rows, dst->GetMemoryRequirements().alignment);

	for (uint32_t face = 0; face < vkRes->GetDesc().DepthOrArraySize; face++)
	{
		for (uint32_t level = 0; level < vkRes->GetDesc().MipLevels; level++)
		{				
			offset = face * totalBytes;
			VkBufferImageCopy& bufferCopyRegion = bufferCopyRegions.emplace_back();
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = level;
			bufferCopyRegion.imageSubresource.baseArrayLayer = face;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = vkRes->GetDesc().Width >> level;
			bufferCopyRegion.imageExtent.height = vkRes->GetDesc().Height >> level;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;
		}
	}

	vkCmdCopyBufferToImage(
		mCommandBuffer,
		vkSrcRes->mBuffer,
		vkRes->mImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(bufferCopyRegions.size()),
		bufferCopyRegions.data());

}

void VulkanGraphicCmdList::SetPipelineState(RHIPipelineState* pipeline)
{
	vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->As<VulkanPipelineState>()->mPipeline);
}

void VulkanGraphicCmdList::SetViewPort(size_t x, size_t y, size_t width, size_t height)
{

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = (float)height;
	viewport.width = (float)width;
	viewport.height = -(float)height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(mCommandBuffer, 0, 1, &viewport);
}

void VulkanGraphicCmdList::BeginRenderPass(RHIRenderPass* pass, RHIFrameBuffer* buffer)
{
	VulkanRenderPass* vkPass = pass->As<VulkanRenderPass>();
	VulkanFrameBuffer* vkBuffer = buffer->As<VulkanFrameBuffer>();
	vk::RenderPassBeginInfo renderPassInfo{};
	renderPassInfo.renderPass = vkPass->mRenderPass;
	renderPassInfo.framebuffer = vkBuffer->mBuffer;
	renderPassInfo.renderArea.setOffset({ 0,0 });
	renderPassInfo.renderArea.setExtent(vkBuffer->GetExtent());

	std::vector<vk::ClearValue> clearColors;
	for (auto& colorDesc : vkPass->mDesc.mColors)
	{
		auto& color = clearColors.emplace_back();
		color.color = std::array<float, 4>{colorDesc.mClearColor.x(), colorDesc.mClearColor.y(), colorDesc.mClearColor.z(), colorDesc.mClearColor.w()};
	}
	
	for (auto& depthDesc : vkPass->mDesc.mDepths)
	{
		auto& depth = clearColors.emplace_back();
		depth.setDepthStencil({ depthDesc.mClearDepth, depthDesc.mClearStencil });
	}	

	renderPassInfo.clearValueCount = clearColors.size();
	renderPassInfo.pClearValues = clearColors.data();

	mCommandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

}

void VulkanGraphicCmdList::EndRenderPass()
{
	vkCmdEndRenderPass(mCommandBuffer);	
}

void VulkanGraphicCmdList::Reset()
{
	if (mClosed)
	{
		vk::CommandBufferBeginInfo beginInfo;
		VULKAN_ASSERT(mCommandBuffer.begin(&beginInfo));
	}		
	mClosed = false;
}

void VulkanGraphicCmdList::BindDesriptorSetExt(RHIBindingSetPtr bindingSet)
{
	if (bindingSet)
	{
		VulkanBindingSet* vkBindingSet = bindingSet->As<VulkanBindingSet>();
		VulkanBindingSetLayout* vkBindingSetLayout = vkBindingSet->mLayout->As<VulkanBindingSetLayout>();

		mCommandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			vkBindingSetLayout->mPipelineLayout,
			0,
			(uint32_t)vkBindingSet->mDescriptors.size(),
			vkBindingSet->mDescriptorSets.data(),
			0,
			nullptr);
	} 
}

void VulkanGraphicCmdList::BeginRender(const RenderPassDesc& passDesc)
{
	std::vector<vk::RenderingAttachmentInfo> attachments;
	std::vector<vk::RenderingAttachmentInfo> depthAttachments;
	std::vector<vk::RenderingAttachmentInfo> stencilAttachments;
	vk::RenderingInfo info;
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
	uint32_t width;
	uint32_t height;
	uint32_t idx = 0;
	for (auto& it : passDesc.mColors)
	{
		auto& attach = attachments.emplace_back();
		
		attach.imageView = passDesc.mColorView[idx]->As<VulkanView>()->mImageView;
		width = passDesc.mColorView[idx]->mBindResource->mResDesc.Width;
		height = passDesc.mColorView[idx]->mBindResource->mResDesc.Height;
		attach.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		attach.loadOp = convertLoad(it.mLoadOp);
		attach.storeOp = convertStore(it.mStoreOp);
		attach.clearValue.color = std::array<float, 4>{it.mClearColor.x(), it.mClearColor.y(), it.mClearColor.z(), it.mClearColor.w()};
	}
	for (auto& it : passDesc.mDepths)
	{
		auto& attach = depthAttachments.emplace_back();
		attach.imageView = passDesc.mDepthStencilView->As<VulkanView>()->mImageView;
		attach.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal;
		attach.loadOp = convertLoad(it.mDepthLoadOp);
		attach.storeOp = convertStore(it.mDepthStoreOp);
		attach.clearValue.setDepthStencil({ it.mClearDepth, it.mClearStencil });
	}

	for (auto& it : passDesc.mDepths)
	{
		auto& attach = stencilAttachments.emplace_back();		
		attach.imageView = passDesc.mDepthStencilView->As<VulkanView>()->mImageView;
		attach.imageLayout = vk::ImageLayout::eStencilAttachmentOptimal;
		attach.loadOp = convertLoad(it.mStencilLoadOp);
		attach.storeOp = convertStore(it.mStencilStoreOp);
		attach.clearValue.setDepthStencil({ it.mClearDepth, it.mClearStencil });
	}
	info.viewMask = 0;
	info.layerCount = 1;
	info.pColorAttachments = attachments.data();
	info.colorAttachmentCount = (uint32_t)attachments.size();
	info.pDepthAttachment = depthAttachments.data();
	info.pStencilAttachment = stencilAttachments.data();

	info.renderArea.setOffset({ 0, 0 });
	info.renderArea.setExtent({ width, height });
	mCommandBuffer.beginRendering(&info);
}

void VulkanGraphicCmdList::EndRender()
{
	mCommandBuffer.endRendering();
}

}



