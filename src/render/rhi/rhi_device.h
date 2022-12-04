#pragma once

#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_types.h"

#include "render/rhi/rhi_cmd_list.h"
#include "render/rhi/rhi_fence.h"
#include "render/rhi/rhi_descriptor.h"
#include "render/rhi/rhi_swapchain.h"
#include "render/rhi/rhi_descriptor_heap.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/rhi_memory.h"
#include "render/rhi/rhi_pipeline.h"
#include "render/rhi/rhi_binding_set_layout.h"
#include "render/rhi/rhi_shader.h"
#include "render/rhi/rhi_frame_buffer.h"


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
	virtual RHIDescriptorPoolPtr CreateDescriptorPool(DescriptorPoolDesc desc)   = 0;
	virtual RHIGraphicCmdListPtr CreateCommondList(RHICmdListType type)          = 0;
	virtual RHIViewPtr CreateView(const ViewDesc&)                               = 0;
	virtual RHIFencePtr CreateFence()                                            = 0;
	virtual RHIFrameBufferPtr CreateFrameBuffer(const FrameBufferDesc& desc)     = 0;
	virtual RHIResourcePtr CreateBufferExt(const RHIBufferDesc& bufDesc)       = 0;


	virtual RHIResourcePtr CreateTextureExt(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc) = 0;

	virtual RHIBindingSetPtr CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) = 0;

	virtual RHIBindingSetLayoutPtr CreateBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys) = 0;

	virtual TRHIPtr<RHIMemory> AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits = 0) = 0;
	
	virtual void CopyInitDataToResource(void* initData, size_t dataSize, RHIResourcePtr sourceDataLayout, RHIResourcePtr resDynamic) = 0;

	
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
// 
// 	bool Create()
// 	{
// 		bool check_error;
// 		check_error = InitDeviceData();
// 		if (!check_error)
// 		{
// 			return check_error;
// 		}
// 		RHIStaticSamplerDesc sampler_member;
// 		// Linear Clamp
// 		sampler_member.Filter = RHIRenderFilter::FILTER_MIN_MAG_MIP_LINEAR;
// 		sampler_member.AddressU = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.AddressV = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.AddressW = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.ShaderRegister = 0;
// 		common_static_sampler.insert(std::pair<LString, RHIStaticSamplerDesc>("SampleTypeClamp", sampler_member));
// 		// Linear Wrap
// 		sampler_member.Filter = RHIRenderFilter::FILTER_MIN_MAG_MIP_LINEAR;
// 		sampler_member.AddressU = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		sampler_member.AddressV = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		sampler_member.AddressW = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		common_static_sampler.insert(std::pair<LString, RHIStaticSamplerDesc>("SampleTypeWrap", sampler_member));
// 		// Linear Wrap
// 		sampler_member.Filter = RHIRenderFilter::FILTER_MIN_MAG_MIP_LINEAR;
// 		sampler_member.AddressU = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		sampler_member.AddressV = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		sampler_member.AddressW = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		common_static_sampler.insert(std::pair<LString, RHIStaticSamplerDesc>("SampleLinearWrap", sampler_member));
// 		// Point Wrap
// 		sampler_member.Filter = RHIRenderFilter::FILTER_MIN_MAG_MIP_POINT;
// 		sampler_member.AddressU = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		sampler_member.AddressV = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		sampler_member.AddressW = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
// 		common_static_sampler.insert(std::pair<LString, RHIStaticSamplerDesc>("SamplePointWrap", sampler_member));
// 		// shadow
// 		sampler_member.Filter = RHIRenderFilter::FILTER_MIN_MAG_MIP_POINT;
// 		sampler_member.AddressU = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.AddressV = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.AddressW = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.ComparisonFunc = RHIComparisionFunc::FuncLessEqual;
// 		common_static_sampler.insert(std::pair<LString, RHIStaticSamplerDesc>("SampleShadow", sampler_member));
// 
// 		// shadow
// 		sampler_member.Filter = RHIRenderFilter::FILTER_MIN_MAG_MIP_LINEAR;
// 		sampler_member.AddressU = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.AddressV = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.AddressW = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_CLAMP;
// 		sampler_member.ComparisonFunc = RHIComparisionFunc::FuncLessEqual;
// 		common_static_sampler.insert(std::pair<LString, RHIStaticSamplerDesc>("SampleShadowPCF", sampler_member));
// 
// 		return true;
// 	}
private:


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
	virtual RHISwapChainPtr CreateSwapChain(
		LWindow* window,
		const RHISwapchainDesc& trarget_window_desc
	) = 0;
	virtual void Wait(RHIFence* fence, uint64_t value) {};
	virtual void Signal(RHIFence* fence, size_t fence_value) = 0;
	virtual bool InitRenderQueue() { return true; };

private:
	friend class RenderModule;
	friend class RenderDevice;
};
}
