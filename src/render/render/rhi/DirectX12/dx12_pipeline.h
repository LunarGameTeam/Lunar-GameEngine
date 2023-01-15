#pragma once
#include "render/render_config.h"
#include "dx12_shader.h"
#include "dx12_binding_set_layout.h"
#include "render/rhi/rhi_pipeline.h"


namespace luna::render
{
class RENDER_API DX12PipelineState : public RHIPipelineState
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline_data;
public:
	DX12PipelineState(const RHIPipelineStateDesc& pso_desc);

	ID3D12PipelineState* GetPipeLine()
	{
		return m_pipeline_data.Get();
	}
	
private:
	bool InitPipeline(const RHIPipelineStateDesc& resource_desc);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetPipelineDesc(const RHIPipelineStateObjectDesc& pipeline_desc,const RenderPassDesc pass_desc);
	D3D12_FILL_MODE GetFillMode(const RHIRasterizerFillMode& pipeline_fill_mode);
	D3D12_CULL_MODE GetCullMode(const RHIRasterizerCullMode& pipeline_cull_mode);
	D3D12_DEPTH_WRITE_MASK GetDepthMask(bool depth_mask);
	D3D12_STENCIL_OP GetStencilOption(const RHIStencilOption& stencil_option);
	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTypologyType(const RHIPrimitiveTopologyType& typology_type);
	D3D12_RENDER_TARGET_BLEND_DESC GetRenderTargetBlendDesc(
		const RHIBlendStateTargetDesc& render_blend_desc);
	D3D12_BLEND GetRenderBlend(const RHIRenderBlend& render_blend);
	D3D12_BLEND_OP GetRenderBlendOption(const RHIBlendOption& render_blend_option);
	D3D12_LOGIC_OP GetRenderBlendLogicOption(const RHIBlendLogicOption& render_blend_logic_option);
	void GetDx12InputElementDesc(const RHIVertexLayout& rhi_layout,LArray<D3D12_INPUT_ELEMENT_DESC> &input_desc_array);
	const char* GetDx12ElementName(VertexElementUsage m_usage);
	int32_t GetDx12ElementIndex(VertexElementUsage m_usage);
	DXGI_FORMAT GetDx12ElementFormat(VertexElementType elementType, uint8_t elementCount);
};
}
