#pragma once
#include "render/pch.h"
#include "render/rhi/vulkan/vulkan_pch.h"

#include "render/rhi/rhi_device.h"
#include "render/render_module.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_swapchain.h"

namespace luna::render
{


class RENDER_API VulkanResource : public RHIResource
{
	
public:
	VulkanResource(uint32_t backBufferId, VulkanSwapChain* swapchain);
	VulkanResource(const RHIBufferDesc& buffer_desc);
	VulkanResource(const SamplerDesc& desc);
	VulkanResource(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc);
	VulkanResource();

	void UpdateUploadBuffer(size_t offset, const void* copy_data, size_t data_size) override;

	void BindMemory(RHIMemory* memory, uint64_t offset) override;


	virtual  void* Map() override;

	virtual void Unmap() override;

	void RefreshMemoryRequirements();
public:		
	size_t mOffset = 0;
	vk::Image mImage = nullptr;
	VkSampler mSampler = nullptr;
	vk::Buffer mBuffer = nullptr;
	vk::Format mVkFormat;

};
}
