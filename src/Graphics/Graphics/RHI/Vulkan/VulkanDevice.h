#pragma once
#include "Graphics/RHI/Vulkan/VulkanPch.h"

#include "Graphics/RHI/RHIDevice.h"

#include "vk_mem_alloc.h"

namespace luna::graphics
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


uint32_t           findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
QueueFamilyIndices findQueueFamilies();

class RENDER_API VulkanMemoryManagerPool
{
	VmaAllocator mAllocator;
public:
	VulkanMemoryManagerPool();
	~VulkanMemoryManagerPool();
	bool Create(const VmaAllocatorCreateInfo& allocatorInfo);
	VkResult BindBufferMemory(RHIHeapType type,const vk::MemoryRequirements& memoryRequire,vk::Buffer& buffer, VmaAllocation& alloc);
	VkResult BindTextureMemory(RHIHeapType type, const vk::MemoryRequirements& memoryRequire, vk::Image& Image, VmaAllocation& alloc);
	VkResult FreeBufferMemory(vk::Buffer& buffer, VmaAllocation& alloc);
	VkResult FreeTextureMemory(vk::Image& Image, VmaAllocation& alloc);
	VkResult MapMemory(VmaAllocation& alloc, void** ppData);
	VkResult UnmapMemory(VmaAllocation& alloc);
private:
	VkResult BindMemoryByRequirement(RHIHeapType type, const vk::MemoryRequirements& memoryRequire, VmaAllocation& alloc);
};

class RENDER_API VulkanDevice : public RHIDevice
{
public:
	VulkanDevice() = default;
	~VulkanDevice() override;


	RHIFencePtr          CreateFence() override;
	RHIDescriptorPoolPtr CreateDescriptorPool(const DescriptorPoolDesc& desc) override;

	RHISinglePoolSingleCmdListPtr CreateSinglePoolSingleCommondList(RHICmdListType type) override;

	RHISinglePoolMultiCmdListPtr CreateSinglePoolMultiCommondList(RHICmdListType type) override;

	RHIMultiFrameCmdListPtr CreateMultiFrameCommondList(size_t frameCount, RHICmdListType type) override;

	RHIViewPtr           CreateView(const ViewDesc&) override;
	RHIResourcePtr       CreateBufferExt(const RHIBufferDesc& buffer_desc) override;

	RHIShaderBlobPtr     CreateShader(const RHIShaderDesc& desc) override;

	RHIPipelineStatePtr CreatePipelineGraphic(
		const RHIPipelineStateGraphDrawDesc& desc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	) override;

	RHIPipelineStatePtr CreatePipelineCompute(const RHIPipelineStateComputeDesc& desc) override;

	RHICmdSignaturePtr   CreateCmdSignature(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc) override;

	RHIMemoryPtr         AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits = 0) override;

	RHIResourcePtr       CreateTextureExt(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc) override;
	RHIBindingSetPtr     CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) override;

	RHIRenderPassPtr     CreateRenderPass(const RenderPassDesc& desc) override;

	RHIFrameBufferPtr    CreateFrameBuffer(const FrameBufferDesc& desc) override;

	RHIResourcePtr       CreateSamplerExt(const SamplerDesc& desc) override;

	RHIBindingSetLayoutPtr CreateBindingSetLayout(const std::vector<RHIBindPoint> & bindKeys) override;


	inline vk::PhysicalDevice GetPhysicalDevice() const
	{
		return mPhysicalDevice; 
	}

	inline vk::Instance GetVkInstance() const
	{
		return mInstance; 
	}
	inline vk::Device GetVKDevice() const 
	{
		return mDevice; 
	}
	inline vk::DispatchLoaderDynamic GetLoader() const
	{ 
		return mDldi; 
	}

	VulkanMemoryManagerPool& GetVulkanVmaPool() { return mDefaultVmaMemoryAllocator; }

	bool mEnableValidation = true;
private:

	bool InitDeviceData() override;
	void pickPhysicalDevice();

	bool checkValidationLayerSupport();	

	void setupDebugMessenger();
	int  rateDeviceSuitability(VkPhysicalDevice device);
	void createLogicalDevice();

	bool checkDeviceExtensionSupport();

private:
	VulkanMemoryManagerPool  mDefaultVmaMemoryAllocator;
	vk::DispatchLoaderDynamic mDldi;
	vk::Instance			 mInstance;
	vk::Device                 mDevice;
	vk::PhysicalDevice		mPhysicalDevice;

	VkDebugUtilsMessengerEXT debugMessenger;
	const                    std::vector< const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

	const                    std::vector< const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME };

};

}
