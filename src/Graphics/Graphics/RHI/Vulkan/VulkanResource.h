#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/Vulkan/VulkanPch.h"

#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RenderModule.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"

namespace luna::graphics
{


class RENDER_API VulkanResource : public RHIResource
{
	
public:
	VulkanResource(uint32_t backBufferId, VulkanSwapChain* swapchain);
	VulkanResource(const RHIBufferDesc& buffer_desc);
	VulkanResource(const SamplerDesc& desc);
	VulkanResource(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc);
	VulkanResource();

	~VulkanResource();

	void ResetResourceBufferSizeDeviceData(size_t newSize) override;

	void UpdateUploadBuffer(size_t offset, const void* copy_data, size_t data_size) override;

	void BindMemory(RHIMemory* memory, uint64_t offset) override;

	void BindMemory(RHIHeapType type) override;

	virtual  void* Map() override;

	virtual void Unmap() override;

	void RefreshMemoryRequirements()const override;
public:		
	size_t mOffset = 0;
	vk::Image mImage = nullptr;
	vk::Sampler mSampler = nullptr;
	vk::Buffer mBuffer = nullptr;
	vk::Format mVkFormat;
private:
	bool AllocByVma = false;
	VmaAllocation mAllocation;
	vk::BufferCreateInfo mBufferInfo = {};
};
}
