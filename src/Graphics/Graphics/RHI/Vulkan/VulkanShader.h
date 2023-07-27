#pragma once

#include "Graphics/RHI/RHIShader.h"
#include "Graphics/RHI/RHIBindingSetLayout.h"


#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <Vulkan/vulkan.hpp>
#include <spirv_reflect.h>
#include <wrl/client.h>
#include <dxc/dxcapi.h>


namespace luna::graphics
{

void InitResources(TBuiltInResource& Resources);

struct DescriptorSetLayoutData
{
	uint32_t set_number;
	VkDescriptorSetLayoutCreateInfo create_info;
	std::vector<VkDescriptorSetLayoutBinding> bindings;
};

class RENDER_API VulkanShaderBlob : public RHIShaderBlob
{
	static LUnorderedMap<RHIShaderType, LString> shader_type_string;
public:
	VulkanShaderBlob(const RHIShaderDesc& desc) :
		RHIShaderBlob(desc)
	{
		InitShader(desc);
	}

	vk::ShaderModule mShaderModule;
	std::vector<RHIBindPoint> mBindings;
	
	//Refleciton
	vk::VertexInputBindingDescription mBindingDescription = {};
	vk::PipelineVertexInputStateCreateInfo mVertexInputState = {};
	std::vector<vk::VertexInputAttributeDescription> mAttributeDescriptions;

	std::vector<SpvReflectDescriptorSet*>& GetVulkanReflection() { return mDescriptorReflection; }
private:
	std::vector<SpvReflectDescriptorSet*> mDescriptorReflection;
	SpvReflectShaderModule mReflectModule = {};

	bool InitShader(const RHIShaderDesc& desc);
	void InitDescriptor(std::vector<uint32_t>& bytes);
	void InitInput(std::vector<uint32_t>& bytes);
};
}