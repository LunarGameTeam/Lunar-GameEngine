#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIBindingSetLayout.h"
#include "Graphics/RenderModule.h"

#include "Graphics/RHI/Vulkan/VulkanPch.h"
#include "VulkanDescriptorPool.h"


namespace luna::render
{



class RENDER_API VulkanBindingSetLayout : public RHIBindingSetLayout
{
public:
	VulkanBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys);


	vk::DescriptorSetLayout mDescriptorSetLayout;
	std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts;
	vk::PipelineLayout mPipelineLayout;
	std::vector<std::map<vk::DescriptorType, size_t>> mDescriptorCountBySet;
private:
};


}
