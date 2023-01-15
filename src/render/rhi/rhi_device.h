#pragma once

#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_types.h"

#include "rhi_binding_set_layout.h"
#include "core/platform/window.h"


namespace luna::render
{
//图形设备单例，用于处理图形资源的分配和管理
class RENDER_API RHIDevice : public RHIObject
{
public:
	RHIDevice() = default;
	virtual ~RHIDevice() = default;

	//device的RHI资源创建接口
	
	virtual RHIShaderBlobPtr CreateShader(const RHIShaderDesc& desc)             = 0;
	virtual RHIPipelineStatePtr CreatePipeline(const RHIPipelineStateDesc& desc) = 0;
	virtual RHIRenderPassPtr CreateRenderPass(const RenderPassDesc& desc) = 0;
	virtual RHIResourcePtr CreateSamplerExt(const SamplerDesc& desc)             = 0;
	virtual RHIDescriptorPoolPtr CreateDescriptorPool(const DescriptorPoolDesc& desc) = 0;
	virtual RHIGraphicCmdListPtr CreateCommondList(RHICmdListType type)          = 0;
	virtual RHIViewPtr CreateView(const ViewDesc&)                               = 0;
	virtual RHIFencePtr CreateFence()                                            = 0;
	virtual RHIFrameBufferPtr CreateFrameBuffer(const FrameBufferDesc& desc)     = 0;
	virtual RHIResourcePtr CreateBufferExt(const RHIBufferDesc& bufDesc)       = 0;


	virtual RHIResourcePtr CreateTextureExt(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc) = 0;

	virtual RHIBindingSetPtr CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) = 0;

	virtual RHIBindingSetLayoutPtr CreateBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys) = 0;

	virtual TRHIPtr<RHIMemory> AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits = 0) = 0;

	
	[[deprecated]]
	virtual bool CheckIfResourceHeapTire2() { return true; };

	[[deprecated]]
	virtual RHIViewPtr CreateDescriptor(
		const RHIViewType& descriptor_type,
		size_t descriptor_offset,
		size_t descriptor_size,
		RHIDescriptorPool* descriptor_heap
	) {	return nullptr;	};

	[[deprecated]]
	virtual size_t CountResourceSizeByDesc(const RHIResDesc& res_desc) { return 0; }

	virtual bool InitDeviceData() = 0;
protected:

	LUnorderedMap<LString, RHIStaticSamplerDesc> common_static_sampler;

	friend class RenderModule;
	friend class RenderDevice;
};

enum class RHIQueueType
{
	eGraphic,
	eTransfer,
	ePresent
};
//图形队列单例,用于提交渲染命令
class RENDER_API RHIRenderQueue : public RHIObject
{
protected:
	RHIRenderQueue() = default;
	virtual ~RHIRenderQueue() = default;

public:
	virtual void ExecuteCommandLists(RHIGraphicCmdList* commond_list_array) = 0;
	virtual RHISwapChainPtr CreateSwapChain(LWindow* window, const RHISwapchainDesc& desc) = 0;
	virtual void Present(RHISwapChain* swapchain) {};
	virtual void Wait(RHIFence* fence, uint64_t value) {};
	virtual void Signal(RHIFence* fence, size_t fence_value) = 0;
	virtual bool InitRenderQueue() { return true; };

private:
	friend class RenderModule;
	friend class RenderDevice;
};
}
