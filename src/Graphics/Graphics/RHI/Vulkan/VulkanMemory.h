#pragma once

#include "Graphics/RHI/RHIMemory.h"
#include "Graphics/RenderModule.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"


#include <Vulkan/vulkan.h>

namespace luna::graphics
{
class RENDER_API VulkanMemory : public RHIMemory
{
public:
	VulkanMemory(const RHIMemoryDesc& desc, uint32_t memoryBits):
		RHIMemory(desc)
	{
		vk::MemoryPropertyFlags properties = {};
		switch (desc.Type)
		{
		case RHIHeapType::Default:
			properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
			break;
		case RHIHeapType::Upload:
			properties = vk::MemoryPropertyFlagBits::eHostVisible  | vk::MemoryPropertyFlagBits::eHostCoherent;
			break;
		case RHIHeapType::Readback:
			properties = vk::MemoryPropertyFlagBits::eHostVisible;
			break;
		case RHIHeapType::Custom:
			break;
		default:
			break;
		}
		vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
		vk::MemoryAllocateInfo alloc_info = {};		
		alloc_info.pNext = NULL;
		alloc_info.allocationSize = desc.SizeInBytes;
		alloc_info.memoryTypeIndex = findMemoryType(memoryBits, properties);
		VULKAN_ASSERT(device.allocateMemory(&alloc_info, nullptr, &mMemory))
	}

	~VulkanMemory() override
	{
		sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().freeMemory(mMemory, nullptr);
	};

	vk::DeviceMemory mMemory;
};
}
