#pragma once

#include "render/rhi/rhi_memory.h"
#include "render/render_module.h"
#include "render/rhi/vulkan/vulkan_device.h"


#include <vulkan/vulkan.h>

namespace luna::render
{
class RENDER_API VulkanMemory : public RHIMemory
{
public:
	VulkanMemory(const RHIMemoryDesc& desc, uint32_t memoryBits):
		RHIMemory(desc)
	{
		VkMemoryPropertyFlags properties = {};
		switch (desc.Type)
		{
		case RHIHeapType::Default:
			properties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			break;
		case RHIHeapType::Upload:
			properties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT  |VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			break;
		case RHIHeapType::Readback:
			properties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			break;
		case RHIHeapType::Custom:
			break;
		default:
			break;
		}
		VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.pNext = NULL;
		alloc_info.allocationSize = desc.SizeInBytes;
		alloc_info.memoryTypeIndex = findMemoryType(memoryBits, properties);
		vkAllocateMemory(device, &alloc_info, nullptr, &mMemory);
	}

	VkDeviceMemory mMemory;
};
}
