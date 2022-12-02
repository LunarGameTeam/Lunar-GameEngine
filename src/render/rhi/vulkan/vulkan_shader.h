#pragma once

#include "render/rhi/rhi_shader.h"
#include "render/rhi/rhi_binding_set_layout.h"


#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <vulkan/vulkan.hpp>
#include <spirv_reflect.h>
#include <wrl/client.h>
#include <dxc/dxcapi.h>


namespace luna::render
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
	VkVertexInputBindingDescription mBindingDescription = {};
	VkPipelineVertexInputStateCreateInfo mVertexInputState = {};
	std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions;

	std::vector<SpvReflectDescriptorSet*>& GetVulkanReflection() { return mDescriptorReflection; }
private:
	std::vector<SpvReflectDescriptorSet*> mDescriptorReflection;
	SpvReflectShaderModule mReflectModule = {};

	bool InitShader(const RHIShaderDesc& desc);
	void InitDescriptor(std::vector<uint32_t>& bytes);
	void InitInput(std::vector<uint32_t>& bytes);
};
}