#include "Graphics/RHI/Vulkan/VulkanBindingSetLayout.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanDescriptorPool.h"
#include "VulkanBindingSet.h"
#include "VulkanView.h"

namespace luna::render
{

VulkanBindingSetLayout::VulkanBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();

	std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> bindingsBySpace;
	std::map<uint32_t, std::vector<vk::DescriptorBindingFlags>> bindings_flags_by_set;

	for (const RHIBindPoint& bind_key : bindKeys)
	{
		decltype(auto) binding = bindingsBySpace[bind_key.mSpace].emplace_back();
		binding.binding = bind_key.mSlot;
		binding.descriptorType = Convert(bind_key.mViewType);
		binding.descriptorCount = bind_key.mCount;
		binding.stageFlags = ConvertShader(bind_key.mShaderVisibility);

		decltype(auto) binding_flag = bindings_flags_by_set[bind_key.mSpace].emplace_back();
	}
	m_descriptor_count_by_set.resize(bindingsBySpace.size());
	for (auto& descriptorSet : bindingsBySpace)
	{
		vk::DescriptorSetLayoutCreateInfo setLaytouCreateInfo = {};		
		std::vector<vk::DescriptorSetLayoutBinding>& setLayoutBindings = descriptorSet.second;
		setLaytouCreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
		setLaytouCreateInfo.pBindings = setLayoutBindings.data();

		vk::DescriptorSetLayoutBindingFlagsCreateInfo layoutFlagsInfo = {};
		layoutFlagsInfo.bindingCount = (uint32_t)bindings_flags_by_set[descriptorSet.first].size();
		layoutFlagsInfo.pBindingFlags = bindings_flags_by_set[descriptorSet.first].data();
		
		
		setLaytouCreateInfo.pNext = &layoutFlagsInfo;

		size_t setIdx = descriptorSet.first;
		if (mDescriptorSetLayouts.size() <= setIdx)
		{
			mDescriptorSetLayouts.resize(setIdx + 1);
			m_descriptor_count_by_set.resize(setIdx + 1);
		}

		auto& descriptorSetLayout = mDescriptorSetLayouts[setIdx];
		VULKAN_ASSERT(device.createDescriptorSetLayout(&setLaytouCreateInfo, nullptr, &descriptorSetLayout));
		
		decltype(auto) descriptor_count = m_descriptor_count_by_set[setIdx];
		for (const auto& binding : descriptorSet.second)
		{
			descriptor_count[binding.descriptorType] += binding.descriptorCount;
		}
	}

	std::vector<vk::DescriptorSetLayout> descriptor_set_layouts;
	for (auto& descriptor_set_layout : mDescriptorSetLayouts)
	{
		if (!descriptor_set_layout)
		{
			vk::DescriptorSetLayoutCreateInfo layout_info = {};			
			VULKAN_ASSERT(device.createDescriptorSetLayout(&layout_info, nullptr, &descriptor_set_layout));
		}

		descriptor_set_layouts.emplace_back(descriptor_set_layout);
	}


	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptor_set_layouts.size();
	pipelineLayoutInfo.pSetLayouts = descriptor_set_layouts.data();
	
	VULKAN_ASSERT(device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &mPipelineLayout));
		
	//std::map<uint32_t, std::vector<vk::DescriptorBindingFlags>> bindings_flags_by_set;

}

}
