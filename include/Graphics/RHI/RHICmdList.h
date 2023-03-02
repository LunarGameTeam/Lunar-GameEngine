#pragma once
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RHI/RHIRenderPass.h"

namespace luna::render
{

struct CommandArgDesc
{
	RHIIndirectArgumentType mArgType;
	UINT mSlotOrIndex;
	UINT mConstantOffset;
	UINT mConstantValueSize;
};
class RENDER_API RHICmdSignature : public RHIObject
{
protected:
	RHIPipelineState* mPipeline;
	LArray<CommandArgDesc> mCommondDescs;
public:
	RHICmdSignature(
		RHIPipelineState* pipeline,
		const LArray<CommandArgDesc> &allCommondDesc
	):mPipeline(pipeline), mCommondDescs(allCommondDesc) {};
	const LArray<CommandArgDesc>& GetDesc() const{ return mCommondDescs;};
};
struct RHICmdArgBufferDataDesc
{
	//绘制参数类型
	RHIIndirectArgumentType mArgType;
	//vb与ib填充这两个参数
	LArray<RHIVertexBufferDesc> mVbPointer;
	RHIResource* mIbPointer;
	//shader输入填充这个参数
	RHIView* mCbvSrvUav;
	//draw instance填充这个参数
	uint32_t mIndexCountPerInstance;
	uint32_t mInstanceCount;
	uint32_t mStartIndexLocation;
	int32_t  mBaseVertexLocation;
	uint32_t mStartInstanceLocation;
	//dispatch填充这个参数
	uint32_t ThreadGroupCountX;
	uint32_t ThreadGroupCountY;
	uint32_t ThreadGroupCountZ;
};

class RENDER_API RHICmdArgBuffer : public RHIObject
{
protected:
	size_t mMaxDrawSize;
	const RHICmdSignature* mCommondBufferDataDescs;
	LArray<RHICmdArgBufferDataDesc> mAllCommand;
public:
	RHICmdArgBuffer(
		size_t maxDrawSize,
		const RHICmdSignature* commondBufferDataDescs
	) :mMaxDrawSize(maxDrawSize), mCommondBufferDataDescs(commondBufferDataDescs) {};
	void UpdateArgData(const LArray<RHICmdArgBufferDataDesc>& argData)
	{
		mAllCommand.clear();
		for (const RHICmdArgBufferDataDesc& eachValue : argData)
		{
			mAllCommand.push_back(eachValue);
		}
		UpdateArgDataImpl(argData);
	}
	const RHICmdSignature* GetCmdSignature() const { return mCommondBufferDataDescs; };
	const LArray<RHICmdArgBufferDataDesc>& GetAllCommand() const { return mAllCommand; };
private:
	virtual void UpdateArgDataImpl(const LArray<RHICmdArgBufferDataDesc>& argData) {};
};
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
	virtual void DrawIndirectCommands(const RHICmdArgBuffer* DrawBuffer) = 0;

	virtual void SetDrawPrimitiveTopology(RHIPrimitiveTopology primitive_topology) = 0;

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
