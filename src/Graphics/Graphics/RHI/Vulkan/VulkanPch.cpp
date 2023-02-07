#include "VulkanPch.h"

namespace luna::render
{

DoubleConverter<RHITextureFormat, vk::Format> sVulkanFormtas =
{
	{RHITextureFormat::R8G8BB8A8_UNORN, vk::Format::eR8G8B8A8Unorm},
	{RHITextureFormat::R32_FLOAT, vk::Format::eR32Sfloat},
	{RHITextureFormat::R8G8B8A8_UNORM_SRGB, vk::Format::eR8G8B8A8Srgb },
	{RHITextureFormat::FORMAT_B8G8R8A8_UNORM, vk::Format::eB8G8R8A8Unorm },
	{RHITextureFormat::FORMAT_B8G8R8A8_UNORM_SRGB, vk::Format::eB8G8R8A8Srgb },
	{RHITextureFormat::D24_UNORM_S8_UINT, vk::Format::eD24UnormS8Uint },
	{RHITextureFormat::D32Float, vk::Format::eD32Sfloat},

};

vk::Format Convert(RHITextureFormat format)
{
	return sVulkanFormtas.Get(format);
}

RHITextureFormat Convert(vk::Format format)
{
	return sVulkanFormtas.Get(format);
}
vk::CompareOp Convert(RHIComparisionFunc func)
{
	switch (func)
	{
	case RHIComparisionFunc::FuncNever:
		return vk::CompareOp::eNever;
	case RHIComparisionFunc::FuncLess:
		return vk::CompareOp::eLess;
	case RHIComparisionFunc::FuncEqual:
		return vk::CompareOp::eEqual;
	case RHIComparisionFunc::FuncLessEqual:
		return vk::CompareOp::eLessOrEqual;
	case RHIComparisionFunc::FuncGreater:
		return vk::CompareOp::eGreater;
	case RHIComparisionFunc::FuncNotEqual:
		return vk::CompareOp::eNotEqual;
	case RHIComparisionFunc::FuncGreaterEuqal:
		return vk::CompareOp::eGreaterOrEqual;
	case RHIComparisionFunc::FuncAlways:
		return vk::CompareOp::eAlways;
	default:
		assert(false);
	}
	return vk::CompareOp::eAlways;
}

DoubleConverter<RHIResDimension, vk::ImageType> sImageTypeConverter =
{
	{RHIResDimension::Texture1D, vk::ImageType::e1D},
	{RHIResDimension::Texture2D, vk::ImageType::e2D},
	{RHIResDimension::Texture3D, vk::ImageType::e3D},
};

vk::ImageType Convert(RHIResDimension dimension)
{
	return sImageTypeConverter.Get(dimension);
}

ENABLE_BITMASK_OPERATORS(vk::ShaderStageFlagBits);
vk::ShaderStageFlagBits ConvertShader(RHIShaderVisibility type)
{
	vk::ShaderStageFlagBits bits = vk::ShaderStageFlagBits::eVertex;
	if (Has(type, RHIShaderVisibility::Visibility_Vertex))
		bits |= vk::ShaderStageFlagBits::eVertex;
	if (Has(type, RHIShaderVisibility::Visibility_Pixel))
		bits |= vk::ShaderStageFlagBits::eFragment;
	if (Has(type, RHIShaderVisibility::Visibility_All))
		bits |= vk::ShaderStageFlagBits::eAll;
	return bits;
}

static std::pair<ResourceState, vk::ImageLayout> mapping[] = {
	{ ResourceState::kCommon, vk::ImageLayout::eGeneral },
	{ ResourceState::kRenderTarget, vk::ImageLayout::eColorAttachmentOptimal},
	{ ResourceState::kUnorderedAccess,  vk::ImageLayout::eGeneral },
	{ ResourceState::kDepthStencilWrite,  vk::ImageLayout::eDepthStencilAttachmentOptimal },
	{ ResourceState::kDepthStencilRead, vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal },
	{ ResourceState::kNonPixelShaderResource, vk::ImageLayout::eShaderReadOnlyOptimal },
	{ ResourceState::kShaderReadOnly, vk::ImageLayout::eShaderReadOnlyOptimal },
	{ ResourceState::kCopyDest, vk::ImageLayout::eTransferDstOptimal },
	{ ResourceState::kCopySource, vk::ImageLayout::eTransferSrcOptimal },
	{ ResourceState::kShadingRateSource, vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR },
	{ ResourceState::kPresent, vk::ImageLayout::ePresentSrcKHR },
	{ ResourceState::kUndefined, vk::ImageLayout::eUndefined },
};

vk::ImageLayout ConvertState(ResourceState state)
{
	vk::ImageLayout res = (vk::ImageLayout)0;
	for (const auto& m : mapping)
	{

		if (state & m.first)
		{
			assert(state == m.first);
			return m.second;
		}
	}
	assert(false);
	return vk::ImageLayout::eGeneral;
}


DoubleConverter<RHIViewType, vk::DescriptorType > sViewDescriptor = {
	{RHIViewType::kConstantBuffer, vk::DescriptorType::eUniformBuffer},
	{RHIViewType::kSampler, vk::DescriptorType::eSampler},
	{RHIViewType::kTexture, vk::DescriptorType::eSampledImage},
};

vk::DescriptorType Convert(RHIViewType view_type)
{
	return sViewDescriptor.Get(view_type);
}

DoubleConverter<RHIPrimitiveTopology, vk::PrimitiveTopology > sPrimitiveTopology = {
	{RHIPrimitiveTopology::TriangleList, vk::PrimitiveTopology::eTriangleList},
	{RHIPrimitiveTopology::LineList, vk::PrimitiveTopology::eLineList},	
};

vk::PrimitiveTopology Convert(RHIPrimitiveTopology primitive)
{
	return sPrimitiveTopology.Get(primitive);
}

}