#include "Graphics/RHI/Vulkan/VulkanBindingSetLayout.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanDescriptorPool.h"
#include "VulkanBindingSet.h"
#include "VulkanView.h"

namespace luna::graphics
{

VulkanBindingSetLayout::VulkanBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys, const std::unordered_map<ShaderParamID, RHIPushConstantValue>& mBindConstKeys)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();

	std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> bindingsBySpace;
	std::map<uint32_t, std::vector<vk::DescriptorBindingFlags>> bindings_flags_by_set;

	for (const RHIBindPoint& it : bindKeys)
	{
		vk::DescriptorSetLayoutBinding& binding = bindingsBySpace[it.mSpace].emplace_back();
		binding.binding = it.mSlot;
		binding.descriptorType = Convert(it.mViewType);
		binding.descriptorCount = it.mCount;
		binding.stageFlags = ConvertShader(it.mShaderVisibility);

		decltype(auto) binding_flag = bindings_flags_by_set[it.mSpace].emplace_back();
	}
	for (auto& it : bindingsBySpace)
	{
		vk::DescriptorSetLayoutCreateInfo setLaytouCreateInfo = {};		
		std::vector<vk::DescriptorSetLayoutBinding>& setLayoutBindings = it.second;
		setLaytouCreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
		setLaytouCreateInfo.pBindings = setLayoutBindings.data();

		vk::DescriptorSetLayoutBindingFlagsCreateInfo layoutFlagsInfo = {};
		layoutFlagsInfo.bindingCount = (uint32_t)bindings_flags_by_set[it.first].size();
		layoutFlagsInfo.pBindingFlags = bindings_flags_by_set[it.first].data();
		
		
		setLaytouCreateInfo.pNext = &layoutFlagsInfo;

		size_t setIdx = it.first;
		if (mDescriptorSetLayouts.size() <= setIdx)
		{
			mDescriptorSetLayouts.resize(setIdx + 1);
			mDescriptorCountBySet.resize(setIdx + 1);
		}

		auto& descriptorSetLayout = mDescriptorSetLayouts[setIdx];
		VULKAN_ASSERT(device.createDescriptorSetLayout(&setLaytouCreateInfo, nullptr, &descriptorSetLayout));
		
		decltype(auto) descriptor_count = mDescriptorCountBySet[setIdx];
		for (const auto& binding : it.second)
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
	int32_t pushConstStructSize = mBindConstKeys.size();
	LArray<vk::PushConstantRange> allConstant;
	for (auto itor : mBindConstKeys)
	{
		vk::PushConstantRange push_constant;
		push_constant.offset = itor.second.mOffset;
		push_constant.size = itor.second.mSize;
		push_constant.stageFlags = vk::ShaderStageFlagBits::eAll;
		allConstant.push_back(push_constant);
	}
	if (allConstant.size() > 0)
	{
		pipelineLayoutInfo.pPushConstantRanges = allConstant.data();
		pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)allConstant.size();
	}
	VULKAN_ASSERT(device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &mPipelineLayout));
		
	//std::map<uint32_t, std::vector<vk::DescriptorBindingFlags>> bindings_flags_by_set;

}

}
