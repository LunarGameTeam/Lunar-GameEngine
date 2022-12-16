#pragma once
#include "render/render_config.h"

#include "render/rhi/vulkan/vulkan_pch.h"
#include "render/rhi/rhi_frame_buffer.h"

#include "render/render_module.h"
#include "render/rhi/vulkan/vulkan_resource.h"

namespace luna::render
{

class RENDER_API VulkanView : public RHIView
{
public:

	VulkanView(const ViewDesc& desc) :
		RHIView(desc)
	{

	};

	void BindResource(RHIResource* buffer_data) override;

	vk::ImageView  mImageView;
	vk::BufferView mBufferView;
	vk::DescriptorImageInfo mDescriptorImage = {};
	vk::DescriptorBufferInfo mDescriptorBuffer = {};

	vk::WriteDescriptorSet mDesciptor;

	TRHIPtr<VulkanResource> mRes;
	uint32_t mColorAttachment = 0;

private:
	void CreateImgeView();
	bool Init(RHIDescriptorPool* descriptor_heap);

};

}
