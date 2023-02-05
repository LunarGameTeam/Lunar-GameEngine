#pragma once
#include "Graphics/RHI/Vulkan/VulkanView.h"


#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RenderModule.h"

namespace luna::render
{


void VulkanView::BindResource(RHIResource* res)
{
	mBindResource = res;
	mRes = res->As<VulkanResource>();
	VkDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	auto dimension = mRes->GetDesc().Dimension;

	switch (mViewDesc.mViewType)
	{
	case RHIViewType::Uniform_Invalid:
		break;
	case RHIViewType::kConstantBuffer:
	{
		mDescriptorBuffer.buffer = mRes->mBuffer;
		mDescriptorBuffer.offset = mViewDesc.mOffset;
		mDescriptorBuffer.range = mViewDesc.mBufferSize;
		mDesciptor.pBufferInfo = &mDescriptorBuffer;
		mDesciptor.pTexelBufferView = &mBufferView;
		break;
	}
	case RHIViewType::kUnorderedAccess:
		break;
	case RHIViewType::kRenderTarget:
	case RHIViewType::kDepthStencil:
		CreateImgeView();
		break;
	case RHIViewType::kTexture:
	{
		CreateImgeView();
		mDescriptorImage.imageView = mImageView;
		mDescriptorImage.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		mDesciptor.pImageInfo = &mDescriptorImage;
		
		break;
	}
	case RHIViewType::kSampler:
	{
		mDescriptorImage.sampler = mRes->mSampler;
		mDesciptor.pImageInfo = &mDescriptorImage;
		break;
	}
	default:
		break;

	}
}

bool VulkanView::Init(RHIDescriptorPool* descriptor_heap)
{
	return true;
}

void VulkanView::CreateImgeView()
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	vk::ImageViewCreateInfo createInfo{};
	createInfo.image = mRes->mImage;	
	switch (mViewDesc.mViewDimension)
	{
	case RHIViewDimension::BufferView:
		break;
	case RHIViewDimension::TextureView1D:
		createInfo.viewType = vk::ImageViewType::e1D;
		break;
	case RHIViewDimension::TextureView1DArray:
		createInfo.viewType = vk::ImageViewType::e1DArray;
		break;
	case RHIViewDimension::TextureView2D:
		createInfo.viewType = vk::ImageViewType::e2D;
		break;
	case RHIViewDimension::TextureView2DArray:
		createInfo.viewType = vk::ImageViewType::e2DArray;
		break;
	case RHIViewDimension::TextureView3D:
		createInfo.viewType = vk::ImageViewType::e3D;
		break;
	case RHIViewDimension::TextureViewCube:
		createInfo.viewType = vk::ImageViewType::eCube;
		break;
	case RHIViewDimension::TextureViewCubeArray:
		createInfo.viewType = vk::ImageViewType::eCubeArray;
		break;
	case RHIViewDimension::TextureView3DArray:
	case RHIViewDimension::Unknown:		
	default:
		assert(false);
		break;
	}
	createInfo.format = mRes->mVkFormat;
	createInfo.components.r = vk::ComponentSwizzle::eIdentity;
	createInfo.components.g = vk::ComponentSwizzle::eIdentity;
	createInfo.components.b = vk::ComponentSwizzle::eIdentity;
	createInfo.components.a = vk::ComponentSwizzle::eIdentity;
	

	if(mViewType == RHIViewType::kDepthStencil)
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
	else
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

	createInfo.subresourceRange.baseMipLevel = mViewDesc.mBaseMipLevel;
	createInfo.subresourceRange.levelCount = mViewDesc.mLevelCount;
	createInfo.subresourceRange.baseArrayLayer = mViewDesc.mBaseArrayLayer;
	createInfo.subresourceRange.layerCount = mViewDesc.mLayerCount;
	if (device.createImageView(&createInfo, nullptr, &mImageView) != vk::Result::eSuccess) {
		throw std::runtime_error("failed to create image views!");
	}
}

VulkanView::~VulkanView()
{
	vk::Device device = sRenderModule->GetDevice<VulkanDevice>()->GetVKDevice();
	if (mImageView)
	{
		device.destroyImageView(mImageView);
		mImageView = nullptr;
	}
	if (mBufferView)
	{
		device.destroyBufferView(mBufferView);
		mBufferView = nullptr;
	}
}

}
