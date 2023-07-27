#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderModule.h"
#include "Graphics/RHI/Vulkan/VulkanRenderQueue.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanFence.h"
#include "Graphics/RHI/Vulkan/VulkanCmdList.h"
#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"


namespace luna::graphics
{


VulkanRenderQueue::VulkanRenderQueue(RHIQueueType type /*= RenderQueueType::eGraphic*/)
{
	QueueFamilyIndices indices = findQueueFamilies();
	if (type == RHIQueueType::eGraphic)
	{
		sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().getQueue(indices.graphicsFamily.value(), 0, &mQueue);
	}
	else if (type == RHIQueueType::eTransfer)
	{
		sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().getQueue(indices.transferFamily.value(), 0, &mQueue);
	}
}

void VulkanRenderQueue::ExecuteCommandLists(RHIGraphicCmdList* commond_list_array)
{
	VulkanGraphicCmdList* vkList = commond_list_array->As<VulkanGraphicCmdList>();

	vk::SubmitInfo submit_info = {};
	
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &vkList->mCommandBuffer;

	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eAllCommands };
	submit_info.pWaitDstStageMask = waitStages;

	VULKAN_ASSERT(mQueue.submit(1, &submit_info, {}));
}

RHISwapChainPtr VulkanRenderQueue::CreateSwapChain(LWindow* window, const RHISwapchainDesc& desc)
{
	RHISwapChainPtr swapchin = CreateRHIObject<VulkanSwapChain>(window, desc);
	swapchin->As<VulkanSwapChain>()->Init();
	return swapchin;
}

void VulkanRenderQueue::Signal(RHIFence* fence, size_t val)
{
	decltype(auto) vk_fence = fence->As<VulkanFence>();
	vk::TimelineSemaphoreSubmitInfo timeline = {};
	timeline.signalSemaphoreValueCount = 1;
	timeline.pSignalSemaphoreValues = &val;

	vk::SubmitInfo submit = {};
	submit.pNext = &timeline;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &vk_fence->mSempahore;
	VULKAN_ASSERT(mQueue.submit(1, &submit, {}));
}

void VulkanRenderQueue::Present(RHISwapChain* swapchain)
{
	uint32_t nowIndex = swapchain->GetNowFrameID();
	vk::PresentInfoKHR present_info = {};
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &swapchain->As<VulkanSwapChain>()->mSwapChain;
	present_info.pImageIndices = &nowIndex;
	present_info.waitSemaphoreCount = 0;	
	VULKAN_ASSERT(mQueue.presentKHR(&present_info));
}

}