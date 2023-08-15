#define VMA_IMPLEMENTATION 
#include"VulkanMemory.h"
namespace luna::graphics
{
	int ceshi()
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;

		//allocatorInfo.physicalDevice = physicalDevice;
		//allocatorInfo.device = device;
		//allocatorInfo.instance = instance;

		VmaAllocator allocator;
		vmaCreateAllocator(&allocatorInfo, &allocator);


		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = 65536;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

		VkBuffer buffer;
		VmaAllocation allocation;
		vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
		return 0;
	}
}