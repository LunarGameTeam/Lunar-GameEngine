#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIFrameBuffer.h"

#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/RHIDescriptorHeap.h"
#include "Graphics/RenderModule.h"

namespace luna::render
{

struct VKDescriptorSetSegment
{
	vk::DescriptorPool pool;
	vk::DescriptorSet set;
	~VKDescriptorSetSegment();
};


class RENDER_API VulkanDescriptorPool : public RHIDescriptorPool
{
public:
	VulkanDescriptorPool(const DescriptorPoolDesc& heapDesc)
	{
		Resize(heapDesc);		
	}

	void Resize(const DescriptorPoolDesc& heapDesc)
	{
		std::vector< vk::DescriptorPoolSize> pool_sizes;
		uint32_t setCount = 0;
		for (auto it : heapDesc.mPoolAllocateSizes)
		{
			vk::DescriptorPoolSize& pool_size = pool_sizes.emplace_back();
			setCount = std::max(setCount, it.second);
			switch (it.first)
			{
			default:
				break;
			case DescriptorHeapType::CBV_SRV_UAV:
				pool_size.type = vk::DescriptorType::eUniformBuffer; 
				pool_size.descriptorCount = it.second;
				break;
			case DescriptorHeapType::SAMPLER:
				pool_size.type = vk::DescriptorType::eSampler;
				pool_size.descriptorCount = it.second;
				break;
			case DescriptorHeapType::RTV:
				pool_size.type = vk::DescriptorType::eSampler;
				pool_size.descriptorCount = it.second;
				break;
			case DescriptorHeapType::DSV:
				pool_size.type = vk::DescriptorType::eSampler;
				pool_size.descriptorCount = it.second;
				break;
			case DescriptorHeapType::NUM_TYPES:
				break;
			};
		}
		vk::DescriptorPoolCreateInfo poolInfo{};
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
		poolInfo.poolSizeCount = (uint32_t)pool_sizes.size();
		poolInfo.pPoolSizes = pool_sizes.data();
		poolInfo.maxSets = setCount;
		VULKAN_ASSERT(sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice().createDescriptorPool(&poolInfo, nullptr, &mPool));
	}

	vk::DescriptorPool mPool = nullptr;
};

RENDER_API VKDescriptorSetSegment* AllocateDescriptorSet(RHIDescriptorPool* pool, vk::DescriptorSetLayout& set_layout, const std::map<vk::DescriptorType, size_t>& count);
}
