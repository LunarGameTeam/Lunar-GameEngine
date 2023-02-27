#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/RHICmdList.h"
#include <Vulkan/vulkan.hpp>

namespace luna::render
{
	class RENDER_API  VulkanCmdSignature : public RHICmdSignature
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> mDxCmdSignature;
	public:
		VulkanCmdSignature(
			RHIPipelineState* pipeline,
			const LArray<CommandArgDesc>& allCommondDesc
		);
		ID3D12CommandSignature* GetValue() { return mDxCmdSignature.Get(); }
	};

class VulkanGraphicCmdList : public RHIGraphicCmdList
{
public:
	vk::CommandPool mCommandPool;
	vk::CommandBuffer mCommandBuffer;

	VulkanGraphicCmdList(RHICmdListType commond_list_type);

	void DrawIndexedInstanced(
		uint32_t IndexCountPerInstance,
		uint32_t InstanceCount,
		uint32_t StartIndexLocation,
		int32_t BaseVertexLocation,
		int32_t StartInstanceLocation) override;
	
	void DrawIndirect() override;

	void SetDrawPrimitiveTopology(RHIPrimitiveTopology primitive_topology) override;

	void ResourceBarrierExt(const ResourceBarrierDesc& desc) override;

	void SetBiningSetLayoutExt(
		RHIBindingSetLayout* root_signature
	) override;

	void BindDescriptorHeap() override;

	void SetVertexBuffer(const LArray<RHIVertexBufferDesc>& vertex_buffer_in, int32_t begin_slot = 0) override;

	void SetIndexBuffer(
		RHIResource* indexRes
	) override;

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

	void SetPipelineState(RHIPipelineState* pipeline) override;

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


	void Reset() override;
	void CloseCommondList() override;

	void BindDesriptorSetExt(RHIBindingSetPtr bindingSet) override;




	void BeginRender(const RenderPassDesc&) override;


	void EndRender() override;

private:

};
}
