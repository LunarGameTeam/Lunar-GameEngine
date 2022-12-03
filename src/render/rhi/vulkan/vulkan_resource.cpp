#include "render/rhi/vulkan/vulkan_resource.h"
#include "render/rhi/vulkan/vulkan_memory.h"

namespace luna::render
{

DoubleConverter<RHITextureFormat, vk::Format> sVulkanFormtas =
{
	{RHITextureFormat::FORMAT_R8G8BB8A8_UNORM, vk::Format::eR8G8B8A8Unorm},
	{RHITextureFormat::FORMAT_R8G8B8A8_UNORM_SRGB, vk::Format::eR8G8B8A8Srgb },
	{RHITextureFormat::FORMAT_B8G8R8A8_UNORM, vk::Format::eB8G8R8A8Unorm },
	{RHITextureFormat::FORMAT_B8G8R8A8_UNORM_SRGB, vk::Format::eB8G8R8A8Srgb },
	{RHITextureFormat::FORMAT_D24_UNORM_S8_UINT, vk::Format::eD24UnormS8Uint },

};

vk::Format Convert(RHITextureFormat format)
{
	return sVulkanFormtas.Get(format);
}

RHITextureFormat Convert(vk::Format format)
{
	return sVulkanFormtas.Get(format);
}


void VulkanResource::UpdateUploadBuffer(size_t offset, const void* copy_data, size_t data_size)
{
	throw std::logic_error("The method or operation is not implemented.");
}

VulkanResource::VulkanResource(uint32_t backBufferId, VulkanSwapChain* swapchain) :
	RHIResource()
{
	mResDesc.Dimension = RHIResDimension::Texture2D;
	mResDesc.mType = ResourceType::kTexture;
	mResDesc.Width = swapchain->GetWidth();
	mResDesc.Height = swapchain->GetHeight();

	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	uint32_t imageCount;

	vkGetSwapchainImagesKHR(device, swapchain->mSwapChain, &imageCount, nullptr);
	std::vector<VkImage> images(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain->mSwapChain, &imageCount, images.data());
	mImage = images[backBufferId];
	mVkFormat = swapchain->mSwapChainImageFormat;
	mResDesc.Format = Convert(mVkFormat);
	RefreshMemoryRequirements();
}

VulkanResource::VulkanResource()
{
	assert(false);
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

VulkanResource::VulkanResource(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc) :
	RHIResource(resDesc)
{
	assert(resDesc.mType == ResourceType::kTexture);
	vk::ImageCreateInfo imageInfo{};
	
	imageInfo.imageType = Convert(resDesc.Dimension);
	imageInfo.extent.width = resDesc.Width;
	imageInfo.extent.height = resDesc.Height;
	imageInfo.extent.depth = 1;
	imageInfo.arrayLayers = resDesc.DepthOrArraySize;
	imageInfo.mipLevels = resDesc.MipLevels;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	
	if (Has(resDesc.mImageUsage, RHIImageUsage::TransferSrcBit))
	{
		imageInfo.usage |= vk::ImageUsageFlagBits::eTransferSrc;
		imageInfo.initialLayout = vk::ImageLayout::ePreinitialized;
	}
	if (Has(resDesc.mImageUsage, RHIImageUsage::TransferDstBit))
	{
		imageInfo.usage |= vk::ImageUsageFlagBits::eTransferDst;
	}
	if (Has(resDesc.mImageUsage, RHIImageUsage::SampledBit))
	{
		imageInfo.usage |= vk::ImageUsageFlagBits::eSampled;
	}
	if (Has(resDesc.mImageUsage, RHIImageUsage::ColorAttachmentBit))
	{
		imageInfo.usage |= vk::ImageUsageFlagBits::eColorAttachment;
	}

	if (Has(resDesc.mImageUsage, RHIImageUsage::DepthStencilBit))
	{
		imageInfo.usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
	}

	mVkFormat = Convert(resDesc.Format);

	imageInfo.format = mVkFormat;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	
	if (device.createImage(&imageInfo, nullptr, &mImage)!= vk::Result::eSuccess) {
		throw std::runtime_error("failed to create image!");
	}

	SetInitialState(ResourceState::kUndefined);
	RefreshMemoryRequirements();
}

VulkanResource::VulkanResource(const RHIBufferDesc& desc)
{
	mResDesc.mType = ResourceType::kBuffer;
	mResDesc.Dimension = RHIResDimension::Buffer;

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	

	vk::BufferCreateInfo bufferInfo{};
	
	bufferInfo.size = desc.mSize;
	
	
	if (Has(desc.mBufferUsage, RHIBufferUsage::TransferSrcBit))
		bufferInfo.usage |= vk::BufferUsageFlagBits::eTransferSrc;
	if (Has(desc.mBufferUsage, RHIBufferUsage::TransferDstBit))
		bufferInfo.usage |= vk::BufferUsageFlagBits::eTransferDst;
	if (Has(desc.mBufferUsage, RHIBufferUsage::UniformBufferBit))
		bufferInfo.usage |= vk::BufferUsageFlagBits::eUniformBuffer;
	if (Has(desc.mBufferUsage, RHIBufferUsage::IndexBufferBit))
		bufferInfo.usage |= vk::BufferUsageFlagBits::eIndexBuffer;
	if (Has(desc.mBufferUsage, RHIBufferUsage::VertexBufferBit))
		bufferInfo.usage |= vk::BufferUsageFlagBits::eVertexBuffer;
	
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;
	
	if (device.createBuffer(&bufferInfo, nullptr, &mBuffer) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create vertex buffer!");
	}
	SetInitialState(ResourceState::kCommon);

	RefreshMemoryRequirements();
}

VulkanResource::VulkanResource(const SamplerDesc& desc)
{
	mResDesc.mType = ResourceType::kSampler;
	mResDesc.Dimension = RHIResDimension::Unknown;

	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VkFilter::VK_FILTER_LINEAR;
	samplerInfo.minFilter = VkFilter::VK_FILTER_LINEAR;
	samplerInfo.anisotropyEnable = false;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.mipmapMode = VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = std::numeric_limits<float>::max();

	/*switch (desc.filter)
	{
	case SamplerFilter::kAnisotropic:
		sampler_desc.Filter = D3D12_FILTER_ANISOTROPIC;
		break;
	case SamplerFilter::kMinMagMipLinear:
		sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case SamplerFilter::kComparisonMinMagMipLinear:
		sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	}*/

	switch (desc.mode)
	{
	case SamplerTextureAddressMode::kWrap:
		samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case SamplerTextureAddressMode::kClamp:
		samplerInfo.addressModeU = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	}

	switch (desc.func)
	{
	case SamplerComparisonFunc::kNever:
		samplerInfo.compareEnable = false;
		samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_NEVER;
		break;
	case SamplerComparisonFunc::kAlways:
		samplerInfo.compareEnable = true;
		samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_ALWAYS;
		break;
	case SamplerComparisonFunc::kLess:
		samplerInfo.compareEnable = true;
		samplerInfo.compareOp = VkCompareOp::VK_COMPARE_OP_LESS;
		break;
	}
	if (vkCreateSampler(device, &samplerInfo, nullptr, &mSampler) != VK_SUCCESS)
		assert(false);
}

void VulkanResource::BindMemory(RHIMemory* memory, uint64_t offset)
{
	mOffset = offset;
	mBindMemory = memory;
	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	if (mResDesc.mType == ResourceType::kBuffer)
	{
		vkBindBufferMemory(device, mBuffer, memory->As<VulkanMemory>()->mMemory, offset);
	}
	else if (mResDesc.mType == ResourceType::kTexture)
	{
		vkBindImageMemory(device, mImage, memory->As<VulkanMemory>()->mMemory, offset);
	}
}

void VulkanResource::RefreshMemoryRequirements()
{
	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	if (mResDesc.mType == ResourceType::kBuffer)
	{
		VkMemoryRequirements memRequirements = {};
		vkGetBufferMemoryRequirements(device, mBuffer, &memRequirements);
		mMemoryLayout.memory_type_bits = memRequirements.memoryTypeBits;
		mMemoryLayout.size = memRequirements.size;
		mMemoryLayout.alignment = memRequirements.alignment;
	}
	else if (mResDesc.mType == ResourceType::kTexture)
	{
		VkMemoryRequirements memRequirements = {};
		vkGetImageMemoryRequirements(device, mImage, &memRequirements);
		mMemoryLayout.memory_type_bits = memRequirements.memoryTypeBits;
		mMemoryLayout.size = memRequirements.size;
		mMemoryLayout.alignment = memRequirements.alignment;
	}
}

void* VulkanResource::Map()
{
	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	void* dst_data;
	vkMapMemory(device, mBindMemory->As<VulkanMemory>()->mMemory, mOffset, mMemoryLayout.size, {}, &dst_data);
	return dst_data;
}

void VulkanResource::Unmap()
{
	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVkDevice();
	vkUnmapMemory(device, mBindMemory->As<VulkanMemory>()->mMemory);
}

}


