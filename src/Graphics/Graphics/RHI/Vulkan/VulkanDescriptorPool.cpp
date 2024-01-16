#include "Graphics/RHI/Vulkan/VulkanDescriptorPool.h"

namespace luna::graphics
{

RENDER_API VKDescriptorSetSegment* AllocateDescriptorSet(RHIDescriptorPool* pool, vk::DescriptorSetLayout& set_layout, const std::map<vk::DescriptorType, size_t>& count)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	VulkanDescriptorPool* vkPool = pool->As<VulkanDescriptorPool>();

	VKDescriptorSetSegment* res = new VKDescriptorSetSegment();
	res->pool = vkPool->mPool;

	vk::DescriptorSetAllocateInfo alloc_info = {};		
	alloc_info.descriptorPool = res->pool;		
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &set_layout;
	auto result = device.allocateDescriptorSets(&alloc_info, &res->set);
	VULKAN_ASSERT(result);
	return res;

}

VKDescriptorSetSegment::~VKDescriptorSetSegment()
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	device.freeDescriptorSets(pool, set);
}

}