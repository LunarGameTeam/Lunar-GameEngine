#include "render/rhi/vulkan/vulkan_binding_set.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_descriptor_pool.h"
#include "render/rhi/vulkan/vulkan_view.h"
#include "render/rhi/vulkan/vulkan_binding_set_layout.h"

namespace luna::render
{


VulkanBindingSet::VulkanBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) :
	mLayout(layout)
{
	VulkanBindingSetLayout* vkLayout = mLayout->As<VulkanBindingSetLayout>();
	decltype(auto) descriptor_set_layouts = vkLayout->mDescriptorSetLayouts;
	decltype(auto) descriptor_count_by_set = vkLayout->m_descriptor_count_by_set;
	for (size_t i = 0; i < descriptor_set_layouts.size(); ++i)
	{
		m_descriptors.push_back(AllocateDescriptorSet(pool, descriptor_set_layouts[i], descriptor_count_by_set[i]));
		mDescriptorSets.push_back(m_descriptors.back().set);
	}
}

void VulkanBindingSet::WriteBindings(const std::vector<BindingDesc>& bindings)
{
	std::vector<vk::WriteDescriptorSet> descriptors;
	for (const BindingDesc& binding : bindings)
	{
		if(binding.mBindTarget.mName == "")
			continue;

		VulkanView* vkViwe = binding.mView->As<VulkanView>();
		vk::WriteDescriptorSet descriptor = vkViwe->mDesciptor;
		descriptor.pNext = nullptr;
		descriptor.descriptorType = Convert(binding.mBindTarget.mViewType);
		descriptor.dstSet = mDescriptorSets[binding.mBindTarget.mSpace];
		descriptor.dstBinding = binding.mBindTarget.mSlot;
		descriptor.dstArrayElement = 0;
		descriptor.descriptorCount = 1;
		if (descriptor.pImageInfo || descriptor.pBufferInfo || descriptor.pTexelBufferView || descriptor.pNext)
		{
			descriptors.emplace_back(descriptor);
		}
	}

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();		
 	device.updateDescriptorSets((uint32_t)descriptors.size(), descriptors.data(), 0, nullptr);

}

const std::vector<vk::DescriptorSet>& VulkanBindingSet::GetDescriptorSets() const
{
	return mDescriptorSets;
}

}
