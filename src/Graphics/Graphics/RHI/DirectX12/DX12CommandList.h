#pragma once
#include "Graphics/RenderConfig.h"
#include "DX12Pipeline.h"

#include "Graphics/RHI/RHICmdList.h"
#include "Graphics/RHI/RHIFrameBuffer.h"

namespace luna::render
{
class RENDER_API DX12CmdSignature : public RHICmdSignature
{
private:
	Microsoft::WRL::ComPtr<ID3D12CommandSignature> mDxCmdSignature;
	size_t mByteStride;
public:
	DX12CmdSignature(
		RHIPipelineState* pipeline,
		const LArray<CommandArgDesc>& allCommondDesc
	);
	ID3D12CommandSignature* GetValue() const { return mDxCmdSignature.Get(); }
	const size_t GetPerElementByte() const { return mByteStride; };
};

class RENDER_API DX12CmdArgBuffer : RHICmdArgBuffer
{
	Microsoft::WRL::ComPtr<ID3D12Resource> mCommandBuffer;
public:
	DX12CmdArgBuffer(
		size_t maxDrawSize,
		const RHICmdSignature* commondBufferDataDescs
	);
	void UpdateArgDataImpl(const LArray<RHICmdArgBufferDataDesc>& argData) override;

};

class RENDER_API DX12GraphicCmdList : public RHIGraphicCmdList
{
public:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDxCmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList1> mDxCmdList;

	void BindDesriptorSetExt(RHIBindingSetPtr bindingSet) override;


	void ResourceBarrierExt(const ResourceBarrierDesc& desc) override;

	void SetDrawPrimitiveTopology(RHIPrimitiveTopology primitive_topology) override;

	void SetBiningSetLayoutExt(
		RHIBindingSetLayout* root_signature
	) override;

	void BindDescriptorHeap() override;

public:
	DX12GraphicCmdList(RHICmdListType commond_list_type);

	inline ID3D12GraphicsCommandList1* GetCommondList() const
	{
		return mDxCmdList.Get();
	}

	void DrawIndexedInstanced(
		uint32_t IndexCountPerInstance,
		uint32_t InstanceCount,
		uint32_t StartIndexLocation,
		int32_t BaseVertexLocation,
		int32_t StartInstanceLocation) override;

	void DrawIndirectCommands(const RHICmdArgBuffer* DrawBuffer) override;

	void SetVertexBuffer(const std::vector<RHIVertexBufferDesc>& vb, int32_t slot = 0) override;

	void SetIndexBuffer(RHIResource* indexRes) override;

	void CopyBufferToBuffer(
		RHIResource* dstRes,
		uint32_t offset,
		RHIResource* srcRes,
		uint32_t srcOffset,
		size_t size
	) override;

	void CopyBufferToTexture(
		RHIResource* target_resource,
		uint32_t target_subresource,
		RHIResource* source_resource,
		uint32_t source_subresource
	) override;

	void SetPipelineState(
		RHIPipelineState* pipeline
	) override;

	void SetViewPort(
		size_t x,
		size_t y,
		size_t width,
		size_t height
	) override;

	void SetScissorRects(
		size_t x,
		size_t y,
		size_t width,
		size_t height
	) override;

	void ClearRTView(
		RHIView* descriptor_rtv,
		LVector4f clear_color, 
		int x = 0,
		int y = 0,
		int width = 0,
		int height = 0
	) override;

	void ClearDSView(
		uint32_t sr_left,
		uint32_t sr_top,
		uint32_t sr_right,
		uint32_t sr_bottom,
		RHIView* descriptor_dsv,		
		float depth,
		uint8_t stencil
	) override;

	void BeginEvent(const LString& event_str) override;
	void EndEvent() override;

	void BeginRenderPass(RHIRenderPass* pass, RHIFrameBuffer* buffer) override;
	void EndRenderPass() override;
	void BeginRender(const RenderPassDesc&) override;

	void Reset() override;
	void CloseCommondList() override;
private:
	D3D12_PRIMITIVE_TOPOLOGY GetDirectXPrimitiveTopology(const RHIPrimitiveTopology& primitive_topology);
	void BindAndClearView(bool ifClearColor,bool ifClearDepth,RHIView* descriptor_rtv, RHIView* descriptor_dsv);
};
}
