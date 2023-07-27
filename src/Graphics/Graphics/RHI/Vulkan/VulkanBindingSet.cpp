#include "Graphics/RHI/Vulkan/VulkanBindingSet.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RHI/Vulkan/VulkanDescriptorPool.h"
#include "Graphics/RHI/Vulkan/VulkanView.h"
#include "Graphics/RHI/Vulkan/VulkanBindingSetLayout.h"

namespace luna::graphics
{


VulkanBindingSet::VulkanBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) :
	mLayout(layout)
{
	VulkanBindingSetLayout* vkLayout = mLayout->As<VulkanBindingSetLayout>();
	decltype(auto) descriptorSetLyaouts = vkLayout->mDescriptorSetLayouts;
	decltype(auto) descriptorCountBySet = vkLayout->mDescriptorCountBySet;
	for (size_t i = 0; i < descriptorSetLyaouts.size(); ++i)
	{
		auto segment = AllocateDescriptorSet(pool, descriptorSetLyaouts[i], descriptorCountBySet[i]);
		mDescriptors.push_back(segment);
		mDescriptorSets.push_back(mDescriptors.back()->set);
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

const LArray<vk::DescriptorSet>& VulkanBindingSet::GetDescriptorSets() const
{
	return mDescriptorSets;
}

VulkanBindingSet::~VulkanBindingSet()
{
	for (VKDescriptorSetSegment* it : mDescriptors)
	{
		delete it;
	}
	mDescriptors.clear();
}

}
