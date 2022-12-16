#pragma once
#include "render/render_config.h"
#include "render/rhi/rhi_binding_set_layout.h"
#include "render/render_module.h"

#include "render/rhi/vulkan/vulkan_pch.h"
#include "vulkan_descriptor_pool.h"


namespace luna::render
{



class RENDER_API VulkanBindingSetLayout : public RHIBindingSetLayout
{
public:
	VulkanBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys);


	vk::DescriptorSetLayout mDescriptorSetLayout;
	std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
	vk::PipelineLayout mPipelineLayout;
	std::vector<std::map<vk::DescriptorType, size_t>> m_descriptor_count_by_set;
private:
};


}
