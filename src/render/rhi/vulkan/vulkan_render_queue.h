#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_device.h"
#include <vulkan/vulkan.hpp>

namespace luna::render
{


class RENDER_API VulkanRenderQueue : public RHIRenderQueue
{
public:
	VulkanRenderQueue(RHIQueueType type = RHIQueueType::eGraphic);
	~VulkanRenderQueue() = default;

	void ExecuteCommandLists(RHIGraphicCmdList* commond_list_array) override;;
	RHISwapChainPtr CreateSwapChain(LWindow* trarget_window, const RHISwapchainDesc& trarget_window_desc) override;
	void Signal(RHIFence* fence, size_t fence_value) override;

	vk::Queue mQueue;	
};
}
