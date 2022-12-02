#include "render/rhi/vulkan/vulkan_shader.h"
#include "render/rhi/vulkan/vulkan_device.h"

#include "render/render_module.h"
#include "spirv_reflect.h"
#include "core/asset/asset_module.h"
#include "render/rhi/shader_compile.h"


namespace luna::render
{

static uint32_t FormatSize(VkFormat format)
{
	uint32_t result = 0;
	switch (format) {
	case VK_FORMAT_UNDEFINED: result = 0; break;
	case VK_FORMAT_R4G4_UNORM_PACK8: result = 1; break;
	case VK_FORMAT_R4G4B4A4_UNORM_PACK16: result = 2; break;
	case VK_FORMAT_B4G4R4A4_UNORM_PACK16: result = 2; break;
	case VK_FORMAT_R5G6B5_UNORM_PACK16: result = 2; break;
	case VK_FORMAT_B5G6R5_UNORM_PACK16: result = 2; break;
	case VK_FORMAT_R5G5B5A1_UNORM_PACK16: result = 2; break;
	case VK_FORMAT_B5G5R5A1_UNORM_PACK16: result = 2; break;
	case VK_FORMAT_A1R5G5B5_UNORM_PACK16: result = 2; break;
	case VK_FORMAT_R8_UNORM: result = 1; break;
	case VK_FORMAT_R8_SNORM: result = 1; break;
	case VK_FORMAT_R8_USCALED: result = 1; break;
	case VK_FORMAT_R8_SSCALED: result = 1; break;
	case VK_FORMAT_R8_UINT: result = 1; break;
	case VK_FORMAT_R8_SINT: result = 1; break;
	case VK_FORMAT_R8_SRGB: result = 1; break;
	case VK_FORMAT_R8G8_UNORM: result = 2; break;
	case VK_FORMAT_R8G8_SNORM: result = 2; break;
	case VK_FORMAT_R8G8_USCALED: result = 2; break;
	case VK_FORMAT_R8G8_SSCALED: result = 2; break;
	case VK_FORMAT_R8G8_UINT: result = 2; break;
	case VK_FORMAT_R8G8_SINT: result = 2; break;
	case VK_FORMAT_R8G8_SRGB: result = 2; break;
	case VK_FORMAT_R8G8B8_UNORM: result = 3; break;
	case VK_FORMAT_R8G8B8_SNORM: result = 3; break;
	case VK_FORMAT_R8G8B8_USCALED: result = 3; break;
	case VK_FORMAT_R8G8B8_SSCALED: result = 3; break;
	case VK_FORMAT_R8G8B8_UINT: result = 3; break;
	case VK_FORMAT_R8G8B8_SINT: result = 3; break;
	case VK_FORMAT_R8G8B8_SRGB: result = 3; break;
	case VK_FORMAT_B8G8R8_UNORM: result = 3; break;
	case VK_FORMAT_B8G8R8_SNORM: result = 3; break;
	case VK_FORMAT_B8G8R8_USCALED: result = 3; break;
	case VK_FORMAT_B8G8R8_SSCALED: result = 3; break;
	case VK_FORMAT_B8G8R8_UINT: result = 3; break;
	case VK_FORMAT_B8G8R8_SINT: result = 3; break;
	case VK_FORMAT_B8G8R8_SRGB: result = 3; break;
	case VK_FORMAT_R8G8B8A8_UNORM: result = 4; break;
	case VK_FORMAT_R8G8B8A8_SNORM: result = 4; break;
	case VK_FORMAT_R8G8B8A8_USCALED: result = 4; break;
	case VK_FORMAT_R8G8B8A8_SSCALED: result = 4; break;
	case VK_FORMAT_R8G8B8A8_UINT: result = 4; break;
	case VK_FORMAT_R8G8B8A8_SINT: result = 4; break;
	case VK_FORMAT_R8G8B8A8_SRGB: result = 4; break;
	case VK_FORMAT_B8G8R8A8_UNORM: result = 4; break;
	case VK_FORMAT_B8G8R8A8_SNORM: result = 4; break;
	case VK_FORMAT_B8G8R8A8_USCALED: result = 4; break;
	case VK_FORMAT_B8G8R8A8_SSCALED: result = 4; break;
	case VK_FORMAT_B8G8R8A8_UINT: result = 4; break;
	case VK_FORMAT_B8G8R8A8_SINT: result = 4; break;
	case VK_FORMAT_B8G8R8A8_SRGB: result = 4; break;
	case VK_FORMAT_A8B8G8R8_UNORM_PACK32: result = 4; break;
	case VK_FORMAT_A8B8G8R8_SNORM_PACK32: result = 4; break;
	case VK_FORMAT_A8B8G8R8_USCALED_PACK32: result = 4; break;
	case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: result = 4; break;
	case VK_FORMAT_A8B8G8R8_UINT_PACK32: result = 4; break;
	case VK_FORMAT_A8B8G8R8_SINT_PACK32: result = 4; break;
	case VK_FORMAT_A8B8G8R8_SRGB_PACK32: result = 4; break;
	case VK_FORMAT_A2R10G10B10_UNORM_PACK32: result = 4; break;
	case VK_FORMAT_A2R10G10B10_SNORM_PACK32: result = 4; break;
	case VK_FORMAT_A2R10G10B10_USCALED_PACK32: result = 4; break;
	case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: result = 4; break;
	case VK_FORMAT_A2R10G10B10_UINT_PACK32: result = 4; break;
	case VK_FORMAT_A2R10G10B10_SINT_PACK32: result = 4; break;
	case VK_FORMAT_A2B10G10R10_UNORM_PACK32: result = 4; break;
	case VK_FORMAT_A2B10G10R10_SNORM_PACK32: result = 4; break;
	case VK_FORMAT_A2B10G10R10_USCALED_PACK32: result = 4; break;
	case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: result = 4; break;
	case VK_FORMAT_A2B10G10R10_UINT_PACK32: result = 4; break;
	case VK_FORMAT_A2B10G10R10_SINT_PACK32: result = 4; break;
	case VK_FORMAT_R16_UNORM: result = 2; break;
	case VK_FORMAT_R16_SNORM: result = 2; break;
	case VK_FORMAT_R16_USCALED: result = 2; break;
	case VK_FORMAT_R16_SSCALED: result = 2; break;
	case VK_FORMAT_R16_UINT: result = 2; break;
	case VK_FORMAT_R16_SINT: result = 2; break;
	case VK_FORMAT_R16_SFLOAT: result = 2; break;
	case VK_FORMAT_R16G16_UNORM: result = 4; break;
	case VK_FORMAT_R16G16_SNORM: result = 4; break;
	case VK_FORMAT_R16G16_USCALED: result = 4; break;
	case VK_FORMAT_R16G16_SSCALED: result = 4; break;
	case VK_FORMAT_R16G16_UINT: result = 4; break;
	case VK_FORMAT_R16G16_SINT: result = 4; break;
	case VK_FORMAT_R16G16_SFLOAT: result = 4; break;
	case VK_FORMAT_R16G16B16_UNORM: result = 6; break;
	case VK_FORMAT_R16G16B16_SNORM: result = 6; break;
	case VK_FORMAT_R16G16B16_USCALED: result = 6; break;
	case VK_FORMAT_R16G16B16_SSCALED: result = 6; break;
	case VK_FORMAT_R16G16B16_UINT: result = 6; break;
	case VK_FORMAT_R16G16B16_SINT: result = 6; break;
	case VK_FORMAT_R16G16B16_SFLOAT: result = 6; break;
	case VK_FORMAT_R16G16B16A16_UNORM: result = 8; break;
	case VK_FORMAT_R16G16B16A16_SNORM: result = 8; break;
	case VK_FORMAT_R16G16B16A16_USCALED: result = 8; break;
	case VK_FORMAT_R16G16B16A16_SSCALED: result = 8; break;
	case VK_FORMAT_R16G16B16A16_UINT: result = 8; break;
	case VK_FORMAT_R16G16B16A16_SINT: result = 8; break;
	case VK_FORMAT_R16G16B16A16_SFLOAT: result = 8; break;
	case VK_FORMAT_R32_UINT: result = 4; break;
	case VK_FORMAT_R32_SINT: result = 4; break;
	case VK_FORMAT_R32_SFLOAT: result = 4; break;
	case VK_FORMAT_R32G32_UINT: result = 8; break;
	case VK_FORMAT_R32G32_SINT: result = 8; break;
	case VK_FORMAT_R32G32_SFLOAT: result = 8; break;
	case VK_FORMAT_R32G32B32_UINT: result = 12; break;
	case VK_FORMAT_R32G32B32_SINT: result = 12; break;
	case VK_FORMAT_R32G32B32_SFLOAT: result = 12; break;
	case VK_FORMAT_R32G32B32A32_UINT: result = 16; break;
	case VK_FORMAT_R32G32B32A32_SINT: result = 16; break;
	case VK_FORMAT_R32G32B32A32_SFLOAT: result = 16; break;
	case VK_FORMAT_R64_UINT: result = 8; break;
	case VK_FORMAT_R64_SINT: result = 8; break;
	case VK_FORMAT_R64_SFLOAT: result = 8; break;
	case VK_FORMAT_R64G64_UINT: result = 16; break;
	case VK_FORMAT_R64G64_SINT: result = 16; break;
	case VK_FORMAT_R64G64_SFLOAT: result = 16; break;
	case VK_FORMAT_R64G64B64_UINT: result = 24; break;
	case VK_FORMAT_R64G64B64_SINT: result = 24; break;
	case VK_FORMAT_R64G64B64_SFLOAT: result = 24; break;
	case VK_FORMAT_R64G64B64A64_UINT: result = 32; break;
	case VK_FORMAT_R64G64B64A64_SINT: result = 32; break;
	case VK_FORMAT_R64G64B64A64_SFLOAT: result = 32; break;
	case VK_FORMAT_B10G11R11_UFLOAT_PACK32: result = 4; break;
	case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: result = 4; break;

	default:
		break;
	}
	return result;
}

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

bool VulkanShaderBlob::InitShader(const RHIShaderDesc& desc)
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	std::vector<uint32_t> shaderBytes;
	Microsoft::WRL::ComPtr<IDxcBlob> code;
	DxcCompile(desc.mType, desc.mContent, shaderBytes, code);


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

	for (auto it : mDescriptorReflection)
	{
		const SpvReflectDescriptorSet& descriptorSet = *(it);
		for (uint32_t idx = 0; idx < descriptorSet.binding_count; ++idx) {

			const SpvReflectDescriptorBinding& descriptorBinding = *(descriptorSet.bindings[idx]);

			if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				mUniformBuffers[descriptorBinding.name].mBufferSize = descriptorBinding.block.size;
				for (uint32_t member_idx = 0; member_idx < descriptorBinding.block.member_count; member_idx++)
				{
					SpvReflectBlockVariable& var = descriptorBinding.block.members[member_idx];
					ConstantBufferVar& cbVar = mUniformBuffers[descriptorBinding.name].mVars[var.name];
					cbVar.mOffset = var.offset;
					cbVar.mSize = var.size;
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
				key.mViewType = RHIViewType::kTexture;
			else if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER)
				key.mViewType = RHIViewType::kSampler;
			else if (descriptorBinding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				key.mViewType = RHIViewType::kTexture;
			else
				assert(false);
			
			key.mCount = 1;
			for (uint32_t i_dim = 0; i_dim < descriptorBinding.array.dims_count; ++i_dim) {

				key.mCount *= descriptorBinding.array.dims[i_dim];
			}
			key.mSpace = descriptorSet.set;
			key.mShaderVisibility = RHIShaderVisibility::Visibility_All;
		}
	}
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
		mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		for (size_t idx = 0; idx < inputs.size(); ++idx) {
			const SpvReflectInterfaceVariable& reflVar = *(inputs[idx]);
			if(reflVar.location == -1)
				continue;
			
			VkVertexInputAttributeDescription& attrDesc = mAttributeDescriptions.emplace_back();
			attrDesc.location = reflVar.location;
			attrDesc.binding = mBindingDescription.binding;
			attrDesc.format = static_cast<VkFormat>(reflVar.format);
			attrDesc.offset = 0;  // final offset computed below after sorting.
		}
		// Sort attributes by location
		std::sort(std::begin(mAttributeDescriptions), std::end(mAttributeDescriptions),
			[](const VkVertexInputAttributeDescription& a, const VkVertexInputAttributeDescription& b) {
				return a.location < b.location; });
		// Compute final offsets of each attribute, and total vertex stride.
		for (auto& attribute : mAttributeDescriptions) {
			uint32_t format_size = FormatSize(attribute.format);
			attribute.offset = mBindingDescription.stride;
			mBindingDescription.stride += format_size;
		}

		mVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
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