#pragma once
#include "LDirectX12Shader.h"
#include "LDirectX12RootSignature.h"
namespace luna
{
	struct DescriptorTypeDesc
	{
		LString name;
		PancyShaderDescriptorType type;
	};
	INIT_REFLECTION_CLASS(DescriptorTypeDesc,
		reflect_data.name,
		reflect_data.type
	);

	struct PipelineStateDescCompute
	{
		LString root_signature_file;
		LString compute_shader_file;
		LVector<DescriptorTypeDesc> descriptor_type;
	};
	INIT_REFLECTION_CLASS(PipelineStateDescCompute,
		reflect_data.root_signature_file,
		reflect_data.compute_shader_file,
		reflect_data.descriptor_type
	);
	INIT_REFLECTION_CLASS(D3D12_RENDER_TARGET_BLEND_DESC,
		reflect_data.BlendEnable,
		reflect_data.LogicOpEnable,
		reflect_data.SrcBlend,
		reflect_data.DestBlend,
		reflect_data.BlendOp,
		reflect_data.SrcBlendAlpha,
		reflect_data.DestBlendAlpha,
		reflect_data.BlendOpAlpha,
		reflect_data.LogicOp,
		reflect_data.RenderTargetWriteMask
	);


	struct PipelineStateDescGraphic
	{
		LString root_signature_file;
		LString vertex_shader_file;
		LString pixel_shader_file;
		LString geometry_shader_file;
		LString hull_shader_file;
		LString domin_shader_file;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
		LVector<DescriptorTypeDesc> descriptor_type;
	};
	INIT_REFLECTION_CLASS_WITH_STATIC_ARRAY(PipelineStateDescGraphic,
		reflect_data.root_signature_file,
		reflect_data.vertex_shader_file,
		reflect_data.pixel_shader_file,
		reflect_data.geometry_shader_file,
		reflect_data.hull_shader_file,
		reflect_data.domin_shader_file,
		reflect_data.pso_desc.RasterizerState.CullMode,
		reflect_data.pso_desc.RasterizerState.FillMode,
		reflect_data.pso_desc.BlendState.AlphaToCoverageEnable,
		reflect_data.pso_desc.BlendState.IndependentBlendEnable,
		reflect_data.pso_desc.BlendState.RenderTarget,
		reflect_data.pso_desc.DepthStencilState.DepthEnable,
		reflect_data.pso_desc.DepthStencilState.DepthWriteMask,
		reflect_data.pso_desc.DepthStencilState.DepthFunc,
		reflect_data.pso_desc.DepthStencilState.StencilEnable,
		reflect_data.pso_desc.DepthStencilState.StencilReadMask,
		reflect_data.pso_desc.DepthStencilState.StencilWriteMask,
		reflect_data.pso_desc.DepthStencilState.FrontFace.StencilFunc,
		reflect_data.pso_desc.DepthStencilState.FrontFace.StencilDepthFailOp,
		reflect_data.pso_desc.DepthStencilState.FrontFace.StencilPassOp,
		reflect_data.pso_desc.DepthStencilState.FrontFace.StencilFailOp,
		reflect_data.pso_desc.DepthStencilState.BackFace.StencilFunc,
		reflect_data.pso_desc.DepthStencilState.BackFace.StencilDepthFailOp,
		reflect_data.pso_desc.DepthStencilState.BackFace.StencilPassOp,
		reflect_data.pso_desc.DepthStencilState.BackFace.StencilFailOp,
		reflect_data.pso_desc.SampleMask,
		reflect_data.pso_desc.PrimitiveTopologyType,
		reflect_data.pso_desc.NumRenderTargets,
		reflect_data.pso_desc.RTVFormats,
		reflect_data.pso_desc.DSVFormat,
		reflect_data.pso_desc.SampleDesc.Count,
		reflect_data.pso_desc.SampleDesc.Quality,
		reflect_data.descriptor_type
	);
	struct LunarPipelineStateDesc
	{
		LunarGraphicPipeLineType pipeline_type;
		PipelineStateDescCompute pipeline_data_compute;
		PipelineStateDescGraphic pipeline_data_graphic;
	};
	INIT_REFLECTION_CLASS(LunarPipelineStateDesc,
		reflect_data.pipeline_type,
		reflect_data.pipeline_data_compute,
		reflect_data.pipeline_data_graphic
	);

	class LDx12GraphicResourcePipelineState :public LTemplateAssert<LunarPipelineStateDesc>
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipeline_data;
		LPtr<LDx12GraphicResourceRootSignature> root_signature_use;
		LUnorderedMap<LunarGraphicShaderType, LPtr<LDx12GraphicResourceShaderBlob>> m_shader_use;
	public:
		LDx12GraphicResourcePipelineState(const luna::LString& resource_name_in, const LunarPipelineStateDesc& assert_desc) :LTemplateAssert<LunarPipelineStateDesc>(resource_name_in, assert_desc), root_signature_use(this)
		{
		};
		LDx12GraphicResourcePipelineState(const luna::LString& resource_name_in, const Json::Value& resource_desc) :LTemplateAssert<LunarPipelineStateDesc>(resource_name_in, resource_desc), root_signature_use(this)
		{
		};
		LDx12GraphicResourcePipelineState(const luna::LString& resource_name_in, const void* resource_desc, const size_t& resource_size) :LTemplateAssert<LunarPipelineStateDesc>(resource_name_in, resource_desc, resource_size), root_signature_use(this)
		{
		};
		ID3D12PipelineState* GetPipeLine()
		{
			return m_pipeline_data.Get();
		}
	private:
		void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override;
		LResult GetVertexShaderInputLayoutDesc(ID3D12ShaderReflection* t_ShaderReflection, LVector<D3D12_INPUT_ELEMENT_DESC>& t_InputElementDescVec);
		virtual luna::LResult InitResorceByDesc(const  LunarPipelineStateDesc& resource_desc) override;
	};
	void InitPipelineJsonReflect();
}