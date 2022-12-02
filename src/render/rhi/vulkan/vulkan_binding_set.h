#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_binding_set_layout.h"
#include "render/render_module.h"

#include <vulkan/vulkan.h>
#include "vulkan_descriptor_pool.h"


namespace luna::render
{

vk::DescriptorType GetDescriptorType(RHIViewType view_type);

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
