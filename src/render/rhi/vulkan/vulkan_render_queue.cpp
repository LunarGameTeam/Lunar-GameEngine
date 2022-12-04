#pragma once
#include "render/pch.h"
#include "render/render_module.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_fence.h"
#include "render/rhi/vulkan/vulkan_cmd_list.h"
#include "render/rhi/vulkan/vulkan_swapchain.h"


namespace luna::render
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

RHISwapChainPtr VulkanRenderQueue::CreateSwapChain(LWindow* trarget_window, const RHISwapchainDesc& trarget_window_desc)
{
	RHISwapChainPtr swapchin = CreateRHIObject<VulkanSwapChain>(trarget_window, trarget_window_desc);
	swapchin->As<VulkanSwapChain>()->Init();
	return swapchin;
}

void VulkanRenderQueue::Signal(RHIFence* fence, size_t fence_value)
{
	decltype(auto) vk_fence = fence->As<VulkanFence>();
	vk::TimelineSemaphoreSubmitInfo timeline_info = {};
	timeline_info.signalSemaphoreValueCount = 1;
	timeline_info.pSignalSemaphoreValues = &fence_value;

	vk::SubmitInfo signal_submit_info = {};
	signal_submit_info.pNext = &timeline_info;
	signal_submit_info.signalSemaphoreCount = 1;
	signal_submit_info.pSignalSemaphores = &vk_fence->mSempahore;
	VULKAN_ASSERT(mQueue.submit(1, &signal_submit_info, {}));
}

}