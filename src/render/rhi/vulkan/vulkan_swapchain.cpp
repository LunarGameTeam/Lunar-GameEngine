#include "render/rhi/rhi_pch.h"

#include "render/rhi/vulkan/vulkan_swapchain.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"
#include "window/window_module.h"
#include "render/render_module.h"



#include <vulkan/vulkan_win32.h>
#include "vulkan_fence.h"
#include "vulkan_resource.h"

namespace luna::render
{

bool VulkanSwapChain::Init()
{
	assert(mWindow != nullptr);
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = mWindow->GetWin32HWND();
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	

	if (vkCreateWin32SurfaceKHR(sRenderModule->GetDevice<VulkanDevice>()->GetVkInstance(), &surfaceCreateInfo, nullptr, &mSurface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	vk::PhysicalDevice mPhysicalDevice = sRenderModule->GetDevice<VulkanDevice>()->GetPhysicalDevice();
	
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	auto surface_formats = mPhysicalDevice.getSurfaceFormatsKHR(mSurface);	
	vk::SurfaceFormatKHR surfaceFormat;
	for (const auto& surface : surface_formats)
	{
		if (surface.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			surfaceFormat = surface.format;
			break;
		}
	}
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice);

	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}
	vk::SwapchainCreateInfoKHR createInfo{};
	createInfo.surface = mSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;		
	
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
	createInfo.imageSharingMode = vk::SharingMode::eExclusive;

	createInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	if(device.createSwapchainKHR(&createInfo, nullptr, &mSwapChain) != vk::Result::eSuccess) {
		throw std::runtime_error("failed to create swap chain!");
	}


	mSwapChainImageFormat = surfaceFormat.format;
	mSwapChainExtent = extent;

	vkGetSwapchainImagesKHR(device, mSwapChain, &imageCount, nullptr);
	mBackBuffers.resize(imageCount);
	mImageAvailable.resize(imageCount);
	mSwapchainRelease.resize(imageCount);
	mFence.resize(imageCount);
	for (uint32_t i = 0; i < mBackBuffers.size(); i++)
	{
		TRHIPtr<VulkanResource> backBufferTexture = CreateRHIObject<VulkanResource>(i, this);
		mBackBuffers[i] = backBufferTexture.get();
		
		vk::SemaphoreCreateInfo semaphore_create_info = {};
		
		vk::FenceCreateInfo fenceInfo{};	

		VULKAN_ASSERT(device.createSemaphore(&semaphore_create_info, nullptr, &mImageAvailable[i]));
		
		VULKAN_ASSERT(device.createSemaphore(&semaphore_create_info, nullptr, &mSwapchainRelease[i]));
		
		VULKAN_ASSERT(device.createFence(&fenceInfo, nullptr, &mFence[i]));
		
	}


	return true;
}

SwapChainSupportDetails VulkanSwapChain::querySwapChainSupport(vk::PhysicalDevice device)
{

	SwapChainSupportDetails details;
	VULKAN_ASSERT(device.getSurfaceCapabilitiesKHR(mSurface, &details.capabilities));
	
	uint32_t formatCount;
	VULKAN_ASSERT(device.getSurfaceFormatsKHR(mSurface, &formatCount, nullptr));
	

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		VULKAN_ASSERT(device.getSurfaceFormatsKHR(mSurface, &formatCount, details.formats.data()));
	}
	uint32_t presentModeCount;
	VULKAN_ASSERT(device.getSurfacePresentModesKHR(mSurface, &presentModeCount, nullptr));

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		VULKAN_ASSERT(device.getSurfacePresentModesKHR(mSurface, &presentModeCount, details.presentModes.data()));
	}
	bool swapChainAdequate = false;
	return details;
}

vk::SurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace ==  vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			return availablePresentMode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else
	{

		vk::Extent2D actualExtent = {
			static_cast<uint32_t>(sWindowModule->GetMainWindow()->GetWindowWidth()),
			static_cast<uint32_t>(sWindowModule->GetMainWindow()->GetWindowHeight())
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

}

void VulkanSwapChain::PresentFrame(RHIFence* fence, uint64_t waitValue)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	auto renderQueue = sRenderModule->GetCmdQueueCore()->As<VulkanRenderQueue>();

	uint64_t tmp = std::numeric_limits<uint64_t>::max();

	vk::TimelineSemaphoreSubmitInfo timeline_info = {};
	timeline_info.waitSemaphoreValueCount = 1;
	timeline_info.pWaitSemaphoreValues = &waitValue;	
	timeline_info.signalSemaphoreValueCount = 1;
	timeline_info.pSignalSemaphoreValues = &tmp;

	vk::SubmitInfo signal_submit_info = {};
	signal_submit_info.pNext = &timeline_info;
	signal_submit_info.waitSemaphoreCount = 1;
	signal_submit_info.pWaitSemaphores = &fence->As<VulkanFence>()->mSempahore;
	vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	signal_submit_info.pWaitDstStageMask = &waitDstStageMask;
	signal_submit_info.signalSemaphoreCount = 1;
	signal_submit_info.pSignalSemaphores = &mSwapchainRelease[mFrameIndex];

	VULKAN_ASSERT(renderQueue->mQueue.submit(1, &signal_submit_info, {}));

	vk::PresentInfoKHR present_info = {};
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &mSwapChain;
	present_info.pImageIndices = &mFrameIndex;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = &mSwapchainRelease[mFrameIndex];
	VULKAN_ASSERT(renderQueue->mQueue.presentKHR(&present_info));	
}

uint32_t VulkanSwapChain::GetNowFrameID()
{
	return mFrameIndex;
}

bool VulkanSwapChain::Reset(const RHIWindowDesc& window_width_in)
{
	return true;
}

uint32_t VulkanSwapChain::NextImage(RHIFence* fence, uint64_t signal_value)
{
	auto renderQueue = sRenderModule->GetCmdQueueCore()->As<VulkanRenderQueue>();
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();

	mPrevFrameIdx = mFrameIndex;
	VULKAN_ASSERT(device.acquireNextImageKHR(mSwapChain, UINT64_MAX, mImageAvailable[mPrevFrameIdx], nullptr, &mFrameIndex));	

	uint64_t tmp = std::numeric_limits<uint64_t>::max();
	vk::TimelineSemaphoreSubmitInfo timeline_info = {};
	timeline_info.waitSemaphoreValueCount = 1;
	timeline_info.pWaitSemaphoreValues = &tmp;
	timeline_info.signalSemaphoreValueCount = 1;
	timeline_info.pSignalSemaphoreValues = &signal_value;
	vk::SubmitInfo signal_submit_info = {};
	signal_submit_info.pNext = &timeline_info;
	signal_submit_info.waitSemaphoreCount = 1;
	signal_submit_info.pWaitSemaphores = &mImageAvailable[mPrevFrameIdx];
	vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
	signal_submit_info.pWaitDstStageMask = &waitDstStageMask;
	signal_submit_info.signalSemaphoreCount = 1;
	signal_submit_info.pSignalSemaphores = &fence->As<VulkanFence>()->mSempahore;
	VULKAN_ASSERT(renderQueue->mQueue.submit(1, &signal_submit_info, {}));
	return mFrameIndex;
}

}