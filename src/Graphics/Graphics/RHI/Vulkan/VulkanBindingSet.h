#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIBindingSetLayout.h"
#include "Graphics/RHI/Vulkan/VulkanPch.h"
#include "VulkanDescriptorPool.h"


namespace luna::graphics
{

vk::DescriptorType Convert(RHIViewType view_type);

class VulkanBindingSet : public RHIBindingSet
{
public:
	VulkanBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout);
	~VulkanBindingSet();
	void WriteBindings(const std::vector<BindingDesc>& bindings);

	const LArray<vk::DescriptorSet>& GetDescriptorSets() const;

	LArray<VKDescriptorSetSegment*> mDescriptors;
	LArray<vk::DescriptorSet> mDescriptorSets;
	RHIBindingSetLayoutPtr mLayout;
private:
};

}
