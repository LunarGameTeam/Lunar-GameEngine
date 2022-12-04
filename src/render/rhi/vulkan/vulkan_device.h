#pragma once

#include "render/rhi/vulkan/vulkan_pch.h"

#include "render/rhi/rhi_device.h"



namespace luna::render
{

vk::DescriptorType Convert(RHIViewType view_type);

struct QueueFamilyIndices
{
	std::optional<uint32_t>         graphicsFamily;
	std::optional<uint32_t>         presentFamily;
	std::optional<uint32_t>         transferFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo);
void CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer);


uint32_t           findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
QueueFamilyIndices findQueueFamilies();

class RENDER_API VulkanDevice : public RHIDevice
{
public:
	VulkanDevice() = default;
	~VulkanDevice() override;


	RHIFencePtr            CreateFence() override;
	RHIDescriptorPoolPtr   CreateDescriptorPool(DescriptorPoolDesc desc) override;
	RHIGraphicCmdListPtr   CreateCommondList(RHICmdListType pipelineType) override;
	RHIViewPtr             CreateView(const ViewDesc&) override;
	RHIResourcePtr         CreateBufferExt(const RHIBufferDesc& buffer_desc) override;

	RHIShaderBlobPtr       CreateShader(const RHIShaderDesc& desc) override;

	RHIPipelineStatePtr    CreatePipeline(const RHIPipelineStateDesc& desc) override;

	RHIMemoryPtr           AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits = 0) override;

	void				   CopyInitDataToResource(void* initData, size_t dataSize, RHIResourcePtr sourceDataLayout, RHIResourcePtr resDynamic) override;

	RHIResourcePtr         CreateTextureExt(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc) override;
	RHIBindingSetPtr       CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) override;

	RHIRenderPassPtr		CreateRenderPass(const RenderPassDesc& desc) override;

	RHIFrameBufferPtr      CreateFrameBuffer(const FrameBufferDesc& desc) override;

	RHIBindingSetLayoutPtr CreateBindingSetLayout(const std::vector<RHIBindPoint> & bindKeys) override;

	RHIResourcePtr         CreateSamplerExt(const SamplerDesc& desc) override;



	VkPhysicalDevice GetPhysicalDevice() { return mPhysicalDevice; }
	VkDevice         GetVkDevice()       { return mDevice; }
	vk::Instance       GetVkInstance() { return mInstance; }
	vk::Device         GetVKDevice() { return mDevice; }
	vk::DispatchLoaderDynamic& GetLoader(){ return mDldi; }

private:

	bool InitDeviceData() override;
	void pickPhysicalDevice();

	bool checkValidationLayerSupport();	

	void setupDebugMessenger();
	int  rateDeviceSuitability(VkPhysicalDevice device);
	void createLogicalDevice();

	bool checkDeviceExtensionSupport();


private:
	vk::DispatchLoaderDynamic mDldi;
	vk::Instance			 mInstance;
	vk::Device                 mDevice;
	vk::PhysicalDevice		mPhysicalDevice;

	VkDebugUtilsMessengerEXT debugMessenger;
	const                    std::vector< const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	const                    bool enableValidationLayers                = true;
	const                    std::vector< const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME };

};
}
