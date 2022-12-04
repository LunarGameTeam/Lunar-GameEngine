#pragma once

#include "render/pch.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_swapchain.h"
#include "render/rhi/vulkan/vulkan_device.h"

#include "window/window_module.h"
#include "render/render_module.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>


namespace luna::render
{

struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

class RENDER_API VulkanSwapChain : public RHISwapChain
{
public:
	VulkanSwapChain(LWindow* window, const RHISwapchainDesc& windowDesc) :
		RHISwapChain(window, windowDesc) {}
	
	void PresentFrame(RHIFence* fence, uint64_t waitValue) override;
	uint32_t GetNowFrameID() override;
	uint32_t NextImage() override;
	bool Reset(const RHISwapchainDesc& desc) override;
	vk::SurfaceKHR GetVkSurface() { return mSurface; }

	vk::SwapchainKHR mSwapChain;
	vk::Format mSwapChainImageFormat;
	bool Init();
private:
	vk::Extent2D     mSwapChainExtent;
	uint32_t mFrameIndex = 0;
	std::vector<vk::Semaphore> mImageAvailable;
	std::vector<vk::Semaphore> mSwapchainRelease;
	std::vector<vk::Fence> mFence;
	uint32_t mPrevFrameIdx = 0;
	VkSurfaceKHR mSurface = nullptr;

	SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

};
}
