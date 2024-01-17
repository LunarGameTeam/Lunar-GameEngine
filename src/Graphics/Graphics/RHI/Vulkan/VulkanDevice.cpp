#define VMA_IMPLEMENTATION 
#include "Graphics/RHI/Vulkan/VulkanDevice.h"
#include "Graphics/RenderModule.h"

#include <stdexcept>
// --- other includes ---
#define VK_USE_PLATFORM_WIN32_KHR

#include <Vulkan/vulkan.h>
#include <Vulkan/vulkan_win32.h>

#include <SDL_vulkan.h>

#include "glslang/Public/ShaderLang.h"

#include "Graphics/RHI/Vulkan/VulkanSwapchain.h"
#include "Graphics/RHI/Vulkan/VulkanFence.h"
#include "Graphics/RHI/Vulkan/VulkanCmdList.h"
#include "Graphics/RHI/Vulkan/VulkanRenderPass.h"
#include "Graphics/RHI/Vulkan/VulkanFrameBuffer.h"
#include "Graphics/RHI/Vulkan/VulkanMemory.h"
#include "Graphics/RHI/Vulkan/VulkanResource.h"
#include "Graphics/RHI/Vulkan/VulkanShader.h"

#include "Graphics/RHI/Vulkan/VulkanDescriptorPool.h"
#include "Graphics/RHI/Vulkan/VulkanView.h"
#include "Graphics/RHI/Vulkan/VulkanPipeline.h"
#include "Graphics/RHI/Vulkan/VulkanBindingSetLayout.h"
#include "Graphics/RHI/Vulkan/VulkanBindingSet.h"
#include "Core/Platform/PlatformModule.h"

namespace luna::graphics
{

VulkanMemoryManagerPool::VulkanMemoryManagerPool()
{
}

VulkanMemoryManagerPool::~VulkanMemoryManagerPool()
{
	vmaDestroyAllocator(mAllocator);
}

bool VulkanMemoryManagerPool::Create(const VmaAllocatorCreateInfo& allocatorInfo)
{
	VkResult createResult = vmaCreateAllocator(&allocatorInfo, &mAllocator);
	if (createResult != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

VkResult VulkanMemoryManagerPool::BindMemoryByRequirement(RHIHeapType type, const vk::MemoryRequirements& memoryRequire, VmaAllocation &alloc)
{
	VkMemoryRequirements memoryRequireResult = memoryRequire;
	VmaAllocationCreateInfo allocInfo = {};
	if (type == RHIHeapType::Default)
	{
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	}
	else if (type == RHIHeapType::Upload)
	{
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	}
	else
	{
		assert(false);
	}
	VkResult res = vmaAllocateMemory(mAllocator, &memoryRequireResult, &allocInfo, &alloc, nullptr);
	return res;
}

VkResult VulkanMemoryManagerPool::BindBufferMemory(RHIHeapType type, const vk::MemoryRequirements& memoryRequire, vk::Buffer& buffer, VmaAllocation& alloc)
{
	VkResult succeed = VkResult::VK_SUCCESS;
	succeed = BindMemoryByRequirement(type, memoryRequire, alloc);
	if (succeed != VkResult::VK_SUCCESS)
	{
		return succeed;
	}
	succeed = vmaBindBufferMemory(mAllocator, alloc, buffer);
	return succeed;
}

VkResult VulkanMemoryManagerPool::BindTextureMemory(RHIHeapType type, const vk::MemoryRequirements& memoryRequire, vk::Image& Image, VmaAllocation& alloc)
{
	VkResult succeed = VkResult::VK_SUCCESS;
	succeed = BindMemoryByRequirement(type, memoryRequire, alloc);
	if (succeed != VkResult::VK_SUCCESS)
	{
		return succeed;
	}
	succeed = vmaBindImageMemory(mAllocator, alloc, Image);
	return succeed;
}

VkResult VulkanMemoryManagerPool::FreeBufferMemory(vk::Buffer& buffer, VmaAllocation& alloc)
{
	vmaDestroyBuffer(mAllocator, buffer, alloc);
	return VkResult::VK_SUCCESS;
}

VkResult VulkanMemoryManagerPool::FreeTextureMemory(vk::Image& Image, VmaAllocation& alloc)
{
	vmaDestroyImage(mAllocator, Image, alloc);
	return VkResult::VK_SUCCESS;
}

VkResult VulkanMemoryManagerPool::MapMemory(VmaAllocation& alloc, void** ppData)
{
	return vmaMapMemory(mAllocator, alloc, ppData);
}

VkResult VulkanMemoryManagerPool::UnmapMemory(VmaAllocation& alloc)
{
	vmaUnmapMemory(mAllocator, alloc);
	return VkResult::VK_SUCCESS;
}

uint32_t findMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	vk::PhysicalDevice physicalDevice = sRenderModule->GetDevice<VulkanDevice>()->GetPhysicalDevice();
	physicalDevice.getMemoryProperties(&memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		typeBits >>= 1;
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

QueueFamilyIndices findQueueFamilies()
{
	vk::PhysicalDevice device = sRenderModule->GetDevice<VulkanDevice>()->GetPhysicalDevice();
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
	device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());	
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) 
		{
			indices.graphicsFamily = i;
			indices.presentFamily = i;
		}
		if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
		{
			indices.transferFamily = i;			
		}
		i++;
	}
	return indices;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void CmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
	static auto func = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(sRenderModule->GetDevice<VulkanDevice>()->GetVkInstance(), "vkCmdBeginDebugUtilsLabelEXT");
	if (func != nullptr) {
		return func(commandBuffer , pLabelInfo);
	}
	else {
		return;
	}
}

void CmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
{
	static auto func = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(sRenderModule->GetDevice<VulkanDevice>()->GetVkInstance(), "vkCmdEndDebugUtilsLabelEXT");
	if (func != nullptr) {
		return func(commandBuffer);
	}
	else {
		return;
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{
	LogError("Device", "validation layer:{0} ", pCallbackData->pMessage);
	return VK_FALSE;
}

void VulkanDevice::setupDebugMessenger()
{
	if (!mEnableValidation) return;

	vk::DebugUtilsMessengerCreateInfoEXT createInfo;

	
	createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
	createInfo.pfnUserCallback = debugCallback;

	mInstance.createDebugUtilsMessengerEXT(createInfo, nullptr, mDldi);
	
}

bool VulkanDevice::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	return false;
}

void VulkanDevice::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	VULKAN_ASSERT(mInstance.enumeratePhysicalDevices(&deviceCount, nullptr));
	
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices) {
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}

	mPhysicalDevice = candidates.rbegin()->second;

	if (!mPhysicalDevice) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}

}

int VulkanDevice::rateDeviceSuitability(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	int score = 0;
	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}
	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;
	}
	
	return score;
}


bool VulkanDevice::InitDeviceData()
{
	vk::ApplicationInfo appInfo;	
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 3, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	unsigned int ext_count = 0;
	if (!SDL_Vulkan_GetInstanceExtensions(sPlatformModule->GetMainWindow()->GetWindow(), &ext_count, nullptr))
	{
		LogError("Device", "Unable to query the number of Vulkan instance extensions");

	}
	// Providing an already created VkInstance and a function pointer resolving vkGetInstanceProcAddr, all functions are fetched

	std::vector<const char*> ext_names(ext_count);
	if (!SDL_Vulkan_GetInstanceExtensions(sPlatformModule->GetMainWindow()->GetWindow(), &ext_count, ext_names.data()))
	{
		LogError("Device", "Unable to query the number of Vulkan instance extension names");
	}
	vk::InstanceCreateInfo createInfo;	
	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	
	if (mEnableValidation) {
		ext_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(ext_names.size());
	createInfo.ppEnabledExtensionNames = ext_names.data();
	createInfo.pApplicationInfo = &appInfo;

	vk::Result result = vk::createInstance(&createInfo, nullptr, &mInstance);
	mDldi = vk::DispatchLoaderDynamic(mInstance, vkGetInstanceProcAddr);
	setupDebugMessenger();
	// use pvkGetPhysicalDeviceFeatures2KHR
	glslang::InitializeProcess();

	pickPhysicalDevice();
	createLogicalDevice();

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
	allocatorInfo.physicalDevice = mPhysicalDevice;
	allocatorInfo.device = mDevice;
	allocatorInfo.instance = mInstance;

	mDefaultVmaMemoryAllocator.Create(allocatorInfo);
	return true;
}

VulkanDevice::~VulkanDevice()
{
	mInstance.destroy();
	mDevice.destroy();	
}

void VulkanDevice::createLogicalDevice()
{

	QueueFamilyIndices indices = findQueueFamilies();
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	vk::DeviceQueueCreateInfo queueCreateInfo;
	float queuePriority = 1.0f;

	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.transferFamily.value() };

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		vk::DeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	
	vk::PhysicalDeviceVulkan12Features device12Features;
	device12Features.timelineSemaphore = true;
	device12Features.bufferDeviceAddress = true;
	device12Features.bufferDeviceAddressCaptureReplay = true;
	vk::PhysicalDeviceVulkan13Features device13Features;
	device13Features.dynamicRendering = true;
	device12Features.pNext = &device13Features;

	vk::PhysicalDeviceFeatures deviceFeatures{};
	vk::DeviceCreateInfo createInfo{};
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.pNext = &device12Features;
	
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();	

	
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	if (mEnableValidation) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	mDevice = mPhysicalDevice.createDevice(createInfo, nullptr);
	
}


bool VulkanDevice::checkDeviceExtensionSupport()
{
	uint32_t extensionCount;
	VULKAN_ASSERT(mPhysicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr));
	
	std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
	VULKAN_ASSERT(mPhysicalDevice.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()));

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

RHIMemoryPtr VulkanDevice::AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits /*= 0*/)
{
	return CreateRHIObject<VulkanMemory>(desc, memoryBits);
}

RHIDescriptorPoolPtr VulkanDevice::CreateDescriptorPool(const DescriptorPoolDesc& desc)
{
	return CreateRHIObject<VulkanDescriptorPool>(desc);
}

RHISinglePoolSingleCmdListPtr VulkanDevice::CreateSinglePoolSingleCommondList(RHICmdListType type)
{
	return CreateRHIObject<VulkanSinglePoolSingleCmdList>(type);
}

RHISinglePoolMultiCmdListPtr VulkanDevice::CreateSinglePoolMultiCommondList(RHICmdListType type)
{
	return CreateRHIObject<VulkanSinglePoolMultiCmdList>(type);
}

RHIMultiFrameCmdListPtr VulkanDevice::CreateMultiFrameCommondList(size_t frameCount, RHICmdListType type)
{
	return CreateRHIObject<VulkanMultiFrameCmdList>(frameCount,type);
}

RHIResourcePtr VulkanDevice::CreateTextureExt(const RHIResDesc& resDesc)
{
	return CreateRHIObject<VulkanResource>(resDesc);
}

RHIResourcePtr VulkanDevice::CreateBufferExt(const RHIBufferDesc& desc)
{
	return CreateRHIObject<VulkanResource>(desc);
}

RHIResourcePtr VulkanDevice::CreateSamplerExt(const SamplerDesc& desc)
{
	return CreateRHIObject<VulkanResource>(desc);
}

RHIFencePtr VulkanDevice::CreateFence()
{
	return CreateRHIObject<VulkanFence>();
}

RHIViewPtr VulkanDevice::CreateView(const ViewDesc& desc)
{
	return CreateRHIObject<VulkanView>(desc);
}

RHIShaderBlobPtr VulkanDevice::CreateShader(const RHIShaderDesc& desc)
{
	return CreateRHIObject<VulkanShaderBlob>(desc);
}

RHIPipelineStatePtr VulkanDevice::CreatePipelineGraphic(
	const RHIPipelineStateGraphDrawDesc& desc,
	const RHIVertexLayout& inputLayout,
	const RenderPassDesc& renderPassDesc
)
{
	LSharedPtr<RHIPipelineStateGraphDrawDesc> curDesc = MakeShared<RHIPipelineStateGraphDrawDesc>();
	*curDesc = desc;
	luna::graphics::RHIPipelineStatePtr vulkanPipeline = CreateRHIObject<VulkanPipelineStateGraphic>(curDesc, inputLayout, renderPassDesc);
	vulkanPipeline->Create(this);
	return vulkanPipeline;
}

RHIPipelineStatePtr VulkanDevice::CreatePipelineCompute(const RHIPipelineStateComputeDesc& desc)
{
	LSharedPtr<RHIPipelineStateComputeDesc> curDesc = MakeShared<RHIPipelineStateComputeDesc>();
	*curDesc = desc;
	luna::graphics::RHIPipelineStatePtr vulkanPipeline = CreateRHIObject<VulkanPipelineStateCompute>(curDesc);
	vulkanPipeline->Create(this);
	return vulkanPipeline;
}

RHICmdSignaturePtr VulkanDevice::CreateCmdSignature(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)
{
	return CreateRHIObject<VulkanCmdSignature>(pipeline, allCommondDesc);
}

RHIBindingSetPtr VulkanDevice::CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout)
{
	return CreateRHIObject<VulkanBindingSet>(pool, layout);
}

RHIRenderPassPtr VulkanDevice::CreateRenderPass(const RenderPassDesc& desc)
{
	return CreateRHIObject<VulkanRenderPass>(desc);
}

RHIFrameBufferPtr VulkanDevice::CreateFrameBuffer(const FrameBufferDesc& desc)
{
	return CreateRHIObject<VulkanFrameBuffer>(desc);
}

RHIBindingSetLayoutPtr VulkanDevice::CreateBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys, const std::unordered_map<ShaderParamID, RHIPushConstantValue>& mBindConstKeys)
{
	return CreateRHIObject<VulkanBindingSetLayout>(bindKeys,mBindConstKeys);
}
}