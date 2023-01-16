#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIBindingSetLayout.h"
#include "Graphics/RenderModule.h"

#include "Graphics/RHI/Vulkan/VulkanPch.h"
#include "VulkanDescriptorPool.h"


namespace luna::render
{

vk::DescriptorType Convert(RHIViewType view_type);

class VulkanBindingSet : public RHIBindingSet
{
public:
	VulkanBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout);

	void WriteBindings(const std::vector<BindingDesc>& bindings);

	const std::vector<vk::DescriptorSet>& GetDescriptorSets() const;

	std::vector<DescriptorSetPool> m_descriptors;
	std::vector<vk::DescriptorSet> mDescriptorSets;
	RHIBindingSetLayoutPtr mLayout;
private:
};

}
