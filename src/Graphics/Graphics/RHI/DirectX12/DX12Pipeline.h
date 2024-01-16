#pragma once
#include "Graphics/RenderConfig.h"
#include "DX12Shader.h"
#include "DX12BindingSetLayout.h"
#include "Graphics/RHI/RHIPipeline.h"


namespace luna::graphics
{
class RENDER_API DX12PipelineStateGraphic : public RHIPipelineStateGraphic
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline_data;
public:
	DX12PipelineStateGraphic(
		LSharedPtr<RHIPipelineStateDescBase> psoDesc,
		const RHIVertexLayout& inputLayout,
		const RenderPassDesc& renderPassDesc
	);

	ID3D12PipelineState* GetPipeLine()
	{
		return m_pipeline_data.Get();
	}
	
private:
	void CreateGraphDrawPipelineImpl(
		RHIPipelineStateGraphDrawDesc* graphPipelineDesc,
		RHIVertexLayout& inputLayout,
		RenderPassDesc& renderPassDesc
	) override;

	void GetDx12InputElementDesc(const RHIVertexLayout& rhi_layout,LArray<D3D12_INPUT_ELEMENT_DESC> &input_desc_array);

	const char* GetDx12ElementName(VertexElementUsage m_usage);

	int32_t GetDx12ElementIndex(VertexElementUsage m_usage);

	DXGI_FORMAT GetDx12ElementFormat(VertexElementType elementType, uint8_t elementCount);
};

class RENDER_API DX12PipelineStateCompute : public RHIPipelineStateCompute
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline_data;
public:
	DX12PipelineStateCompute(
		LSharedPtr<RHIPipelineStateDescBase> psoDesc
	);
	ID3D12PipelineState* GetPipeLine()
	{
		return m_pipeline_data.Get();
	}
private:
	void CreateComputePipelineImpl(
		RHIPipelineStateComputeDesc* computePipelineDesc
	) override;
};

}
