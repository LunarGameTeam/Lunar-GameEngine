#pragma once
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_ptr.h"
#include "render/rhi/rhi_pipeline.h"
#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/rhi_descriptor.h"
#include "render/rhi/rhi_render_pass.h"

namespace luna::render
{


//命令分配器
class RENDER_API RHICmdAllocator : public RHIObject
{
	RHICmdListType mCmdAllocatorType;
public:
	RHICmdAllocator(const RHICmdListType& commond_allocator_type) :
		mCmdAllocatorType(commond_allocator_type)
	{
	}

	const RHICmdListType& GetPipelineType()
	{
		return mCmdAllocatorType;
	}

	virtual void ResetAllocator() = 0;
};


//commond list, 用于录制渲染命令
class RENDER_API RHIGraphicCmdList : public RHIObject
{
protected:
	RHICmdListState mCmdListState;
	RHICmdListType mCmdListType;

public:
	RHIGraphicCmdList(RHICmdListType listType = RHICmdListType::Graphic3D) : 
		mCmdListState(RHICmdListState::State_Null),
		mCmdListType(listType)
	{	}


	virtual void BeginEvent(const LString& event_str) = 0;
	virtual void EndEvent() = 0;
	
	virtual void Reset() = 0;
	virtual void CloseCommondList() = 0;

	inline RHICmdListType GetPipelineType() const {	return mCmdListType; }
	

	virtual void SetVertexBuffer(const std::vector<RHIVertexBufferDesc>& vb, int32_t slot = 0) = 0;
	virtual void SetIndexBuffer(RHIResource* indexRes) = 0;

	virtual void DrawIndexedInstanced(
		uint32_t IndexCountPerInstance,
		uint32_t InstanceCount,
		uint32_t StartIndexLocation,
		int32_t BaseVertexLocation,
		int32_t StartInstanceLocation) = 0;


	virtual void SetDrawPrimitiveTopology(const RHIPrimitiveTopology& primitive_topology) = 0;

	virtual void CopyBufferToBuffer(
		RHIResource* dstRes,
		uint32_t offset,
		RHIResource* srcRes,
		uint32_t srcOffset,
		size_t size
	) = 0;

	virtual void CopyBufferToTexture(
		RHIResource* dstRes,
		uint32_t dstSubRes,
		RHIResource* srcRes,
		uint32_t srcSubRes
	) = 0;

	virtual void BindDesriptorSetExt(RHIBindingSetPtr bindingSet) = 0;
	virtual void SetBiningSetLayoutExt(RHIBindingSetLayout* root_signature) = 0;
	virtual void BindDescriptorHeap() = 0;

	virtual void ResourceBarrierExt(const ResourceBarrierDesc& desc) {};

	virtual void BeginRender(const RenderPassDesc&) {};
	virtual void EndRender() {};
	virtual void BeginRenderPass(RHIRenderPass* pass, RHIFrameBuffer* buffer) = 0;
	virtual void EndRenderPass() = 0;

	virtual void SetPipelineState(RHIPipelineState* pipeline) = 0;
	virtual void SetViewPort(size_t x, size_t y, size_t width, size_t height) = 0;
	virtual void SetScissorRects(size_t x, size_t y, size_t width,size_t height) = 0;

	//宽高为0会清空整个View
	virtual void ClearRTView(
		RHIView* descriptor_rtv,
		LVector4f clear_color, 
		int x = 0,
		int y = 0,
		int width = 0,
		int height = 0
	) = 0;

	//宽高为0会清空整个View
	virtual void ClearDSView(
		uint32_t sr_left,
		uint32_t sr_top,
		uint32_t sr_right,
		uint32_t sr_bottom,
		RHIView* descriptor_dsv,
		float depth,
		uint8_t stencil
	) = 0;


protected:
	bool mClosed = false;

};
}
