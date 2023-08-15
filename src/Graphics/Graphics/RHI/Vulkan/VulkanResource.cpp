#include "Graphics/RHI/Vulkan/VulkanResource.h"
#include "Graphics/RHI/Vulkan/VulkanMemory.h"

namespace luna::graphics
{

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

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	uint32_t imageCount;

	vkGetSwapchainImagesKHR(device, swapchain->mSwapChain, &imageCount, nullptr);	
	std::vector<vk::Image> images(imageCount);
	VULKAN_ASSERT(device.getSwapchainImagesKHR(swapchain->mSwapChain, &imageCount, images.data()));

	mImage = images[backBufferId];
	mVkFormat = swapchain->mSwapChainImageFormat;
	mResDesc.Format = Convert(mVkFormat);
	RefreshMemoryRequirements();
}

VulkanResource::VulkanResource()
{
	assert(false);
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
	if (imageInfo.arrayLayers % 6 == 0)
		imageInfo.flags |= vk::ImageCreateFlagBits::eCubeCompatible;

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

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	
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

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	

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
	if (Has(desc.mBufferUsage, RHIBufferUsage::StructureBuffer))
		bufferInfo.usage |= vk::BufferUsageFlagBits::eStorageBuffer;
	
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;
	
	if (device.createBuffer(&bufferInfo, nullptr, &mBuffer) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create Vulkan gpu buffer!");
	}
	SetInitialState(ResourceState::kCommon);

	RefreshMemoryRequirements();
}

VulkanResource::VulkanResource(const SamplerDesc& desc)
{
	mResDesc.mType = ResourceType::kSampler;
	mResDesc.Dimension = RHIResDimension::Unknown;

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	vk::SamplerCreateInfo samplerInfo = {};

	samplerInfo.magFilter = vk::Filter::eNearest;
	samplerInfo.minFilter = vk::Filter::eNearest;
	samplerInfo.anisotropyEnable = false;
	samplerInfo.maxAnisotropy = 16;

	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = std::numeric_limits<float>::max();
	

	switch (desc.mode)
	{
	case SamplerTextureAddressMode::kWrap:
		samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
		break;
	case SamplerTextureAddressMode::kClamp:
		samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
		samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
		break;
	}

	switch (desc.func)
	{
	case SamplerComparisonFunc::kNever:
		samplerInfo.compareEnable = false;
		samplerInfo.compareOp = vk::CompareOp::eNever;
		break;
	case SamplerComparisonFunc::kAlways:
		samplerInfo.compareEnable = true;
		samplerInfo.compareOp = vk::CompareOp::eAlways;
		break;
	case SamplerComparisonFunc::kLess:
		samplerInfo.compareEnable = true;
		samplerInfo.compareOp = vk::CompareOp::eLess;
		break;
	}
	VULKAN_ASSERT(device.createSampler(&samplerInfo, nullptr, &mSampler));
}

VulkanResource::~VulkanResource()
{
	if (AllocByVma)
	{
		VulkanMemoryManagerPool& vmaPool = sRenderModule->GetDevice<VulkanDevice>()->GetVulkanVmaPool();
		if (mResDesc.mType == ResourceType::kBuffer)
		{
			vmaPool.FreeBufferMemory(mBuffer, mAllocation);
		}
		else if (mResDesc.mType == ResourceType::kTexture)
		{
			vmaPool.FreeTextureMemory(mImage, mAllocation);
		}
		return;
	}

	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	if (mImage)
		device.destroyImage(mImage);
	if (mBuffer)
		device.destroy(mBuffer);
	if (mSampler)
		device.destroy(mSampler);

}

void VulkanResource::BindMemory(RHIMemory* memory, uint64_t offset)
{
	mOffset = offset;
	mBindMemory = memory;
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	if (mResDesc.mType == ResourceType::kBuffer)
	{
		device.bindBufferMemory(mBuffer, memory->As<VulkanMemory>()->mMemory, offset);
	}
	else if (mResDesc.mType == ResourceType::kTexture)
	{
		device.bindImageMemory(mImage, memory->As<VulkanMemory>()->mMemory, offset);
	}
}

void VulkanResource::BindMemory(RHIHeapType type)
{
	vk::MemoryRequirements memRequirements = {};
	memRequirements.memoryTypeBits = mMemoryLayout.memory_type_bits;
	memRequirements.size = mMemoryLayout.size;
	memRequirements.alignment = mMemoryLayout.alignment;
	VulkanMemoryManagerPool& vmaPool = sRenderModule->GetDevice<VulkanDevice>()->GetVulkanVmaPool();
	if (mResDesc.mType == ResourceType::kBuffer)
	{
		vmaPool.BindBufferMemory(type,memRequirements, mBuffer, mAllocation);
	}
	else if (mResDesc.mType == ResourceType::kTexture)
	{
		vmaPool.BindTextureMemory(type,memRequirements, mImage, mAllocation);
	}
	AllocByVma = true;
}

void VulkanResource::RefreshMemoryRequirements()
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	if (mResDesc.mType == ResourceType::kBuffer)
	{
		vk::MemoryRequirements memRequirements = {};
		device.getBufferMemoryRequirements(mBuffer, &memRequirements);
		mMemoryLayout.memory_type_bits = memRequirements.memoryTypeBits;
		mMemoryLayout.size = memRequirements.size;
		mMemoryLayout.alignment = memRequirements.alignment;
	}
	else if (mResDesc.mType == ResourceType::kTexture)
	{
		vk::MemoryRequirements memRequirements = {};
		device.getImageMemoryRequirements(mImage, &memRequirements);
		mMemoryLayout.memory_type_bits = memRequirements.memoryTypeBits;
		mMemoryLayout.size = memRequirements.size;
		mMemoryLayout.alignment = memRequirements.alignment;
	}
}

void* VulkanResource::Map()
{
	void* dst_data;
	if (AllocByVma)
	{
		VulkanMemoryManagerPool& vmaPool = sRenderModule->GetDevice<VulkanDevice>()->GetVulkanVmaPool();
		vmaPool.MapMemory(mAllocation, &dst_data);
		return dst_data;
	}
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	VULKAN_ASSERT(device.mapMemory(mBindMemory->As<VulkanMemory>()->mMemory, mOffset, mMemoryLayout.size, {}, &dst_data));
	return dst_data;
}

void VulkanResource::Unmap()
{
	if (AllocByVma)
	{
		VulkanMemoryManagerPool& vmaPool = sRenderModule->GetDevice<VulkanDevice>()->GetVulkanVmaPool();
		vmaPool.UnmapMemory(mAllocation);
		return;
	}
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	device.unmapMemory(mBindMemory->As<VulkanMemory>()->mMemory);
}

}


