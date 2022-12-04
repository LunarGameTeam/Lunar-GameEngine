#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/vulkan/vulkan_pch.h"

#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_fence.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/render_module.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"

namespace luna::render
{



class RENDER_API VulkanFence : public RHIFence
{
public:
	VulkanFence()
	{
		vk::SemaphoreTypeCreateInfo timelineCreateInfo = {};
		timelineCreateInfo.initialValue = 0;
		timelineCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;

		vk::SemaphoreCreateInfo createInfo;		
		createInfo.pNext = &timelineCreateInfo;		
		VULKAN_ASSERT(sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().createSemaphore(&createInfo, nullptr, &mSempahore));		
	}

	vk::Semaphore mSempahore;

	size_t IncSignal(RHIRenderQueue* queue) override
	{
		vk::Queue vkQueue = queue->As<VulkanRenderQueue>()->mQueue;
		VkTimelineSemaphoreSubmitInfo timelineInfo;
		timelineInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
		timelineInfo.pNext = NULL;
		timelineInfo.waitSemaphoreValueCount = 0;
		timelineInfo.pWaitSemaphoreValues = &mValue;
		timelineInfo.signalSemaphoreValueCount = 1;
		timelineInfo.pSignalSemaphoreValues = &mValue;

		vk::SubmitInfo submitInfo;
		submitInfo.pNext = &timelineInfo;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &mSempahore;
		submitInfo.commandBufferCount = 0;
		submitInfo.pCommandBuffers = 0;

		VULKAN_ASSERT(vkQueue.submit(1, &submitInfo, VK_NULL_HANDLE));
		
		mValue += 1;
		return mValue - 1;
	}


	uint64_t GetCompletedValue() override
	{
		
		uint64_t count;
		VULKAN_ASSERT(sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().getSemaphoreCounterValue(mSempahore, &count));		
		return count;
	}

	void Wait(size_t value) override
	{
		vk::SemaphoreWaitInfo waitInfo;
		waitInfo.pNext = NULL;
		waitInfo.semaphoreCount = 1;
		waitInfo.pSemaphores = &mSempahore;
		waitInfo.pValues = &value;
		VULKAN_ASSERT(sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().waitSemaphores(&waitInfo, UINT64_MAX));		
	}

};
}
