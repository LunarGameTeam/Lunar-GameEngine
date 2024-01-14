#include "Graphics/RHI/Vulkan/VulkanShader.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"

#include "Graphics/RenderModule.h"
#include "spirv_reflect.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/RHI/ShaderCompile.h"

#include <Vulkan/vulkan_format_traits.hpp>
namespace luna::graphics
{


void InitResources(TBuiltInResource& Resources)
{
	Resources.maxLights = 32;
	Resources.maxClipPlanes = 6;
	Resources.maxTextureUnits = 32;
	Resources.maxTextureCoords = 32;
	Resources.maxVertexAttribs = 64;
	Resources.maxVertexUniformComponents = 4096;
	Resources.maxVaryingFloats = 64;
	Resources.maxVertexTextureImageUnits = 32;
	Resources.maxCombinedTextureImageUnits = 80;
	Resources.maxTextureImageUnits = 32;
	Resources.maxFragmentUniformComponents = 4096;
	Resources.maxDrawBuffers = 32;
	Resources.maxVertexUniformVectors = 128;
	Resources.maxVaryingVectors = 8;
	Resources.maxFragmentUniformVectors = 16;
	Resources.maxVertexOutputVectors = 16;
	Resources.maxFragmentInputVectors = 15;
	Resources.minProgramTexelOffset = -8;
	Resources.maxProgramTexelOffset = 7;
	Resources.maxClipDistances = 8;
	Resources.maxComputeWorkGroupCountX = 65535;
	Resources.maxComputeWorkGroupCountY = 65535;
	Resources.maxComputeWorkGroupCountZ = 65535;
	Resources.maxComputeWorkGroupSizeX = 1024;
	Resources.maxComputeWorkGroupSizeY = 1024;
	Resources.maxComputeWorkGroupSizeZ = 64;
	Resources.maxComputeUniformComponents = 1024;
	Resources.maxComputeTextureImageUnits = 16;
	Resources.maxComputeImageUniforms = 8;
	Resources.maxComputeAtomicCounters = 8;
	Resources.maxComputeAtomicCounterBuffers = 1;
	Resources.maxVaryingComponents = 60;
	Resources.maxVertexOutputComponents = 64;
	Resources.maxGeometryInputComponents = 64;
	Resources.maxGeometryOutputComponents = 128;
	Resources.maxFragmentInputComponents = 128;
	Resources.maxImageUnits = 8;
	Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	Resources.maxCombinedShaderOutputResources = 8;
	Resources.maxImageSamples = 0;
	Resources.maxVertexImageUniforms = 0;
	Resources.maxTessControlImageUniforms = 0;
	Resources.maxTessEvaluationImageUniforms = 0;
	Resources.maxGeometryImageUniforms = 0;
	Resources.maxFragmentImageUniforms = 8;
	Resources.maxCombinedImageUniforms = 8;
	Resources.maxGeometryTextureImageUnits = 16;
	Resources.maxGeometryOutputVertices = 256;
	Resources.maxGeometryTotalOutputComponents = 1024;
	Resources.maxGeometryUniformComponents = 1024;
	Resources.maxGeometryVaryingComponents = 64;
	Resources.maxTessControlInputComponents = 128;
	Resources.maxTessControlOutputComponents = 128;
	Resources.maxTessControlTextureImageUnits = 16;
	Resources.maxTessControlUniformComponents = 1024;
	Resources.maxTessControlTotalOutputComponents = 4096;
	Resources.maxTessEvaluationInputComponents = 128;
	Resources.maxTessEvaluationOutputComponents = 128;
	Resources.maxTessEvaluationTextureImageUnits = 16;
	Resources.maxTessEvaluationUniformComponents = 1024;
	Resources.maxTessPatchComponents = 120;
	Resources.maxPatchVertices = 32;
	Resources.maxTessGenLevel = 64;
	Resources.maxViewports = 16;
	Resources.maxVertexAtomicCounters = 0;
	Resources.maxTessControlAtomicCounters = 0;
	Resources.maxTessEvaluationAtomicCounters = 0;
	Resources.maxGeometryAtomicCounters = 0;
	Resources.maxFragmentAtomicCounters = 8;
	Resources.maxCombinedAtomicCounters = 8;
	Resources.maxAtomicCounterBindings = 1;
	Resources.maxVertexAtomicCounterBuffers = 0;
	Resources.maxTessControlAtomicCounterBuffers = 0;
	Resources.maxTessEvaluationAtomicCounterBuffers = 0;
	Resources.maxGeometryAtomicCounterBuffers = 0;
	Resources.maxFragmentAtomicCounterBuffers = 1;
	Resources.maxCombinedAtomicCounterBuffers = 1;
	Resources.maxAtomicCounterBufferSize = 16384;
	Resources.maxTransformFeedbackBuffers = 4;
	Resources.maxTransformFeedbackInterleavedComponents = 64;
	Resources.maxCullDistances = 8;
	Resources.maxCombinedClipAndCullDistances = 8;
	Resources.maxSamples = 4;
	Resources.maxMeshOutputVerticesNV = 256;
	Resources.maxMeshOutputPrimitivesNV = 512;
	Resources.maxMeshWorkGroupSizeX_NV = 32;
	Resources.maxMeshWorkGroupSizeY_NV = 1;
	Resources.maxMeshWorkGroupSizeZ_NV = 1;
	Resources.maxTaskWorkGroupSizeX_NV = 32;
	Resources.maxTaskWorkGroupSizeY_NV = 1;
	Resources.maxTaskWorkGroupSizeZ_NV = 1;
	Resources.maxMeshViewCountNV = 4;
	Resources.limits.nonInductiveForLoops = 1;
	Resources.limits.whileLoops = 1;
	Resources.limits.doWhileLoops = 1;
	Resources.limits.generalUniformIndexing = 1;
	Resources.limits.generalAttributeMatrixVectorIndexing = 1;
	Resources.limits.generalVaryingIndexing = 1;
	Resources.limits.generalSamplerIndexing = 1;
	Resources.limits.generalVariableIndexing = 1;
	Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

void ReflectionVar(CBufferVar& cbVar ,const SpvReflectBlockVariable& var)
{
	if (var.array.dims_count > 0)
	{
		cbVar.mIsArray = true;
		cbVar.mElementSize = var.size;
		for (int i = 0; i < var.array.dims_count; i++)
		{
			cbVar.mElementSize = cbVar.mElementSize / var.array.dims[i];
		}
	}
	if (var.members)
	{
		cbVar.mIsStruct = true;
		for (int i = 0; i < var.member_count; ++i)
		{
			SpvReflectBlockVariable& var2 = var.members[i];
			size_t hash2 = LString(var.name).Hash();
			CBufferVar& cbVar2 = cbVar.mStructVars[hash2];
			ReflectionVar(cbVar2, var2);
		}
	}
	cbVar.mName = var.name;
	cbVar.mOffset = var.offset;
	cbVar.mSize = var.size;
}

bool VulkanShaderBlob::InitShader(const RHIShaderDesc& desc)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	std::vector<uint32_t> shaderBytes;
	Microsoft::WRL::ComPtr<IDxcBlob> code;
	DxcCompile(desc.mType, desc.mName, desc.mContent, shaderBytes, code, desc.mShaderMacros);


	InitDescriptor(shaderBytes);
	InitInput(shaderBytes);

	vk::ShaderModuleCreateInfo createInfo{};
	createInfo.codeSize = shaderBytes.size() * sizeof(uint32_t);	
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBytes.data());
	VULKAN_ASSERT(device.createShaderModule(&createInfo, nullptr, &mShaderModule));

	std::map<std::tuple<uint32_t, uint32_t>, RHIBindPoint> result;
	for (RHIBindPoint& bindKey :  mBindings)
	{
		mBindPoints[bindKey.mName.Hash()] = bindKey;
	}
	for (RHIPushConstantValue& bindKey : mPushConstants)
	{
		mBindConstants[bindKey.mName.Hash()] = bindKey;
	}
	for (auto it : mDescriptorReflection)
	{
		const SpvReflectDescriptorSet& descriptorSet = *(it);
		for (uint32_t idx = 0; idx < descriptorSet.binding_count; ++idx) {

			const SpvReflectDescriptorBinding& descriptorBinding = *(descriptorSet.bindings[idx]);
			ShaderParamID id = LString(descriptorBinding.name).Hash();
			auto& uniform = mUniformBuffers[id];
			uniform.mName = descriptorBinding.name;
			if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				uniform.mSize = descriptorBinding.block.size;
				for (uint32_t member_idx = 0; member_idx < descriptorBinding.block.member_count; member_idx++)
				{
					SpvReflectBlockVariable& var = descriptorBinding.block.members[member_idx];
					ShaderParamID varHash = LString(var.name).Hash();
					CBufferVar& cbVar = mUniformBuffers[id].mVars[varHash];
					ReflectionVar(cbVar, var);					
				}

			}
		}
	}

	return true;
}

void VulkanShaderBlob::InitDescriptor(std::vector<uint32_t>& shaderBytes)
{
	SpvReflectResult result = spvReflectCreateShaderModule(shaderBytes.size() * 4, shaderBytes.data(), &mReflectModule);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	uint32_t count = 0;
	result = spvReflectEnumerateDescriptorSets(&mReflectModule, &count, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	mDescriptorReflection.resize(count);
	result = spvReflectEnumerateDescriptorSets(&mReflectModule, &count, mDescriptorReflection.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	uint32_t pushConstantCount = 0;
	result = spvReflectEnumeratePushConstants(&mReflectModule, &pushConstantCount, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	mPushConstReflection.resize(pushConstantCount);
	result = spvReflectEnumeratePushConstants(&mReflectModule, &pushConstantCount, mPushConstReflection.data());
	for (size_t idx = 0; idx < mDescriptorReflection.size(); ++idx)
	{
		const SpvReflectDescriptorSet& descriptorSet = *(mDescriptorReflection[idx]);
		for (uint32_t binding_idx = 0; binding_idx < descriptorSet.binding_count; ++binding_idx) {
			RHIBindPoint& key = mBindings.emplace_back();
			const SpvReflectDescriptorBinding& descriptorBinding = *(descriptorSet.bindings[binding_idx]);
			key.mSlot = descriptorBinding.binding;
			key.mName = descriptorBinding.name;
			if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				key.mViewType = RHIViewType::kConstantBuffer;		
				key.mSize = descriptorBinding.block.size;
			}
			else if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE)
			{
				key.mViewType = RHIViewType::kTexture;				
			}
			else if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER)
				key.mViewType = RHIViewType::kSampler;
			else if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
			{
				key.mViewType = RHIViewType::kTexture;
			}
			else if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				key.mViewType = RHIViewType::kStructuredBuffer;
			}
			else
				assert(false);
			
			key.mCount = 1;
			for (uint32_t i_dim = 0; i_dim < descriptorBinding.array.dims_count; ++i_dim)
			{

				key.mCount *= descriptorBinding.array.dims[i_dim];
			}
			key.mSpace = descriptorSet.set;
			key.mShaderVisibility = RHIShaderVisibility::Visibility_All;
		}
	}
	for (size_t idx = 0; idx < mPushConstReflection.size(); ++idx)
	{
		RHIPushConstantValue& newValue = mPushConstants.emplace_back();
		newValue.mName = mPushConstReflection[idx]->name;
		newValue.mSize = mPushConstReflection[idx]->size;
		newValue.mOffset = mPushConstReflection[idx]->offset;
		for (int32_t memberIndex = 0; memberIndex < mPushConstReflection[idx]->member_count; ++memberIndex)
		{
			RHIPushConstantMember newMember;
			newMember.mName = mPushConstReflection[idx]->members[memberIndex].name;
			newMember.mSize = mPushConstReflection[idx]->members[memberIndex].size;
			newMember.mOffset = mPushConstReflection[idx]->members[memberIndex].offset;
			newValue.mValueMember.insert({ newMember.mName ,newMember });
		}
	}
	return;
}


void VulkanShaderBlob::InitInput(std::vector<uint32_t>& bytes)
{
	SpvReflectResult result;

	uint32_t count = 0;

	result = spvReflectEnumerateInputVariables(&mReflectModule, &count, NULL);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	std::vector<SpvReflectInterfaceVariable*> inputs(count);
	result = spvReflectEnumerateInputVariables(&mReflectModule, &count, inputs.data());

	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	if (mReflectModule.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) {
		// Demonstrates how to generate all necessary data structures to populate
		// a VkPipelineVertexInputStateCreateInfo structure, given the module's
		// expected input variables.
		//
		// Simplifying assumptions:
		// - All vertex input attributes are sourced from a single vertex buffer,
		//   bound to VB slot 0.
		// - Each vertex's attribute are laid out in ascending order by location.
		// - The format of each attribute matches its usage in the shader;
		//   float4 -> VK_FORMAT_R32G32B32A32_FLOAT, etc. No attribute compression is applied.
		// - All attributes are provided per-vertex, not per-instance.
		
		mBindingDescription.binding = 0;
		mBindingDescription.stride = 0;  // computed below
		mBindingDescription.inputRate = vk::VertexInputRate::eVertex;
		for (size_t idx = 0; idx < inputs.size(); ++idx) {
			const SpvReflectInterfaceVariable& reflVar = *(inputs[idx]);
			if(reflVar.location == -1)
				continue;
			
			vk::VertexInputAttributeDescription& attrDesc = mAttributeDescriptions.emplace_back();
			attrDesc.location = reflVar.location;
			attrDesc.binding = mBindingDescription.binding;
			attrDesc.format = static_cast<vk::Format>(reflVar.format);
			attrDesc.offset = 0;  // final offset computed below after sorting.
		}
		// Sort attributes by location
		std::sort(std::begin(mAttributeDescriptions), std::end(mAttributeDescriptions),
			[](const vk::VertexInputAttributeDescription& a, const vk::VertexInputAttributeDescription& b) {
				return a.location < b.location; });
		// Compute final offsets of each attribute, and total vertex stride.
		for (auto& attribute : mAttributeDescriptions) {
			uint32_t format_size = vk::packed(attribute.format);
			attribute.offset = mBindingDescription.stride;
			mBindingDescription.stride += format_size;
		}

		mVertexInputState.vertexBindingDescriptionCount = 1;
		mVertexInputState.pVertexBindingDescriptions = &mBindingDescription;
		mVertexInputState.vertexAttributeDescriptionCount = (uint32_t)mAttributeDescriptions.size();
		mVertexInputState.pVertexAttributeDescriptions = mAttributeDescriptions.data();
		// Nothing further is done with attribute_descriptions or binding_description
		// in this sample. A real application would probably derive this information from its
		// mesh format(s); a similar mechanism could be used to ensure mesh/shader compatibility.
	}
}

}