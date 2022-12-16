#pragma once
#include "render/render_config.h"
#include "render/rhi/rhi_binding_set_layout.h"
#include "render/render_module.h"

#include "render/rhi/vulkan/vulkan_pch.h"
#include "vulkan_descriptor_pool.h"


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
