#include "Graphics/RHI/Vulkan/VulkanDescriptorPool.h"

namespace luna::render
{

RENDER_API DescriptorSetPool AllocateDescriptorSet(RHIDescriptorPool* pool, vk::DescriptorSetLayout& set_layout, const std::map<vk::DescriptorType, size_t>& count)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	VulkanDescriptorPool* vkPool = pool->As<VulkanDescriptorPool>();

	DescriptorSetPool res = {};
	res.pool = vkPool->mPool;

	vk::DescriptorSetAllocateInfo alloc_info = {};		
	alloc_info.descriptorPool = res.pool;		
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &set_layout;
	VULKAN_ASSERT(device.allocateDescriptorSets(&alloc_info, &res.set));
	
	return res;

}

}