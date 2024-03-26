#pragma once
#include "Graphics/RenderConfig.h"

#include "Graphics/RHI/Vulkan/VulkanPch.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/Vulkan/VulkanResource.h"

namespace luna::graphics
{

class RENDER_API VulkanView : public RHIView
{
public:

	VulkanView(const ViewDesc& desc) :
		RHIView(desc)
	{

	};

	~VulkanView();

	void BindResource(RHIResource* buffer_data) override;

	vk::ImageView  mImageView = nullptr;
	vk::BufferView mBufferView = nullptr;
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
