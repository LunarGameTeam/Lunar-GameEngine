#include "render/rhi/DirectX12/dx12_pipeline.h"
#include "core/asset/asset_module.h"
#include "render/render_module.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/rhi/DirectX12/dx12_render_pass.h"

using Microsoft::WRL::ComPtr;


namespace luna::render
{
DX12PipelineState::DX12PipelineState(const RHIPipelineStateDesc& pso_desc) : RHIPipelineState(pso_desc)
{
	InitPipeline(pso_desc);
}

bool DX12PipelineState::GetVertexShaderInputLayoutDesc(ID3D12ShaderReflection* t_ShaderReflection,
                                                          LVector<D3D12_INPUT_ELEMENT_DESC>& t_InputElementDescVec)
{
	HRESULT hr;
	/*
	http://www.cnblogs.com/macom/archive/2013/10/30/3396419.html
	*/
	D3D12_SHADER_DESC t_ShaderDesc;
	hr = t_ShaderReflection->GetDesc(&t_ShaderDesc);
	if (FAILED(hr))
	{		
		LogError("Device", "get desc of shader reflect error");
		return false;
	}
	unsigned int t_ByteOffset = 0;
	for (int i = 0; i != t_ShaderDesc.InputParameters; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC t_SP_DESC;
		t_ShaderReflection->GetInputParameterDesc(i, &t_SP_DESC);

		D3D12_INPUT_ELEMENT_DESC t_InputElementDesc;
		t_InputElementDesc.SemanticName = t_SP_DESC.SemanticName;
		t_InputElementDesc.SemanticIndex = t_SP_DESC.SemanticIndex;
		t_InputElementDesc.InputSlot = 0;
		t_InputElementDesc.AlignedByteOffset = t_ByteOffset;
		t_InputElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		t_InputElementDesc.InstanceDataStepRate = 0;
		if (t_SP_DESC.Mask == 1)
		{
			if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32_UINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32_SINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			t_ByteOffset += 4;
		}
		else if (t_SP_DESC.Mask <= 3)
		{
			if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			t_ByteOffset += 8;
		}
		else if (t_SP_DESC.Mask <= 7)
		{
			if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			t_ByteOffset += 12;
		}
		else if (t_SP_DESC.Mask <= 15)
		{
			if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			}
			else if (t_SP_DESC.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
			{
				t_InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			t_ByteOffset += 16;
		}
		t_InputElementDescVec.push_back(t_InputElementDesc);
	}
	return true;
}

bool DX12PipelineState::InitPipeline(const RHIPipelineStateDesc& psoDesc)
{
	ID3D12Device* dx12Device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
	assert(dx12Device != nullptr);
	//根据格式文件创建管线资源
	if (psoDesc.mType == RHICmdListType::Graphic3D)
	{
		const RenderPipelineStateDescGraphic &desc = psoDesc.mGraphicDesc;
				//读取pipeline的信息
		D3D12_GRAPHICS_PIPELINE_STATE_DESC dx12Desc = GetPipelineDesc(desc.mPipelineStateDesc, desc.mRenderPass);

		//获取着色器
		RHIShaderBlob* shaders[] =
		{
			desc.mPipelineStateDesc.mVertexShader,
			desc.mPipelineStateDesc.mPixelShader,
			desc.mPipelineStateDesc.mGeometryShader,
			desc.mPipelineStateDesc.mHullShader,
			desc.mPipelineStateDesc.mDominShader,
		};

		dx12Desc.VS = CD3DX12_SHADER_BYTECODE(desc.mPipelineStateDesc.mVertexShader->As<DX12ShaderBlob>()->GetDX12Blob());
		dx12Desc.PS = CD3DX12_SHADER_BYTECODE(desc.mPipelineStateDesc.mPixelShader->As<DX12ShaderBlob>()->GetDX12Blob());
		if(desc.mPipelineStateDesc.mGeometryShader)
			dx12Desc.GS = CD3DX12_SHADER_BYTECODE(desc.mPipelineStateDesc.mGeometryShader->As<DX12ShaderBlob>()->GetDX12Blob());
		if (desc.mPipelineStateDesc.mHullShader)
			dx12Desc.HS = CD3DX12_SHADER_BYTECODE(desc.mPipelineStateDesc.mHullShader->As<DX12ShaderBlob>()->GetDX12Blob());
		if (desc.mPipelineStateDesc.mDominShader)
			dx12Desc.DS = CD3DX12_SHADER_BYTECODE(desc.mPipelineStateDesc.mDominShader->As<DX12ShaderBlob>()->GetDX12Blob());

		BindKeyMap bindingKeys;
		for (int i = 0; i < 5; ++i)
		{
			if (shaders[i] == nullptr)
				continue;
			RHIShaderBlobPtr curShdaer = shaders[i];			
			DX12ShaderBlob * dxShaderBlob = dynamic_cast<DX12ShaderBlob*>(curShdaer.get());
			RHIShaderType type = curShdaer->GetDesc().mType;
			mShaders[type] = curShdaer;
			for (auto it : curShdaer->mBindPoints)
			{
				bindingKeys[std::make_pair(it.second.mSpace, it.second.mSlot)] = it.second;
			}
		}

		std::vector<RHIBindPoint> bindKeysVector;
		for (auto it : bindingKeys)
		{
			bindKeysVector.push_back(it.second);
		}


		mBindingSetLayout = sRenderModule->GetRHIDevice()->CreateBindingSetLayout(bindKeysVector);
		DX12BindingSetLayout* dx12BindingSetLayout = mBindingSetLayout->As<DX12BindingSetLayout>();
		dx12Desc.pRootSignature = dx12BindingSetLayout->GetRootSignature();

		//根据顶点着色器获取默认的顶点输入格式
		DX12ShaderBlob* vertexShader = mPSODesc.mGraphicDesc.mPipelineStateDesc.mVertexShader->As<DX12ShaderBlob>();

		ID3D12ShaderReflection* vertex_shader_reflection = vertexShader->GetDX12ShaderReflection();
		LVector<D3D12_INPUT_ELEMENT_DESC> input_desc_array;
		GetVertexShaderInputLayoutDesc(vertex_shader_reflection, input_desc_array);

		D3D12_INPUT_ELEMENT_DESC* vertex_desc = new D3D12_INPUT_ELEMENT_DESC[input_desc_array.size()];
		for (int32_t element_index = 0; element_index < input_desc_array.size(); ++element_index)
		{
			vertex_desc[element_index] = input_desc_array[element_index];
		}
		dx12Desc.InputLayout.pInputElementDescs = vertex_desc;
		dx12Desc.InputLayout.NumElements = static_cast<UINT>(input_desc_array.size());
		//参数填写完毕后，开始创建图形管线
		HRESULT hr = dx12Device->CreateGraphicsPipelineState(&dx12Desc, IID_PPV_ARGS(&m_pipeline_data));
		assert(SUCCEEDED(hr));		
	}
	return true;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC DX12PipelineState::GetPipelineDesc(const RHIPipelineStateObjectDesc& pipeline_desc, const RenderPassDesc pass_desc)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_out = {};
	desc_out.RasterizerState.CullMode = GetCullMode(pipeline_desc.RasterizerState.CullMode);
	desc_out.RasterizerState.FillMode = GetFillMode(pipeline_desc.RasterizerState.FillMode);
	desc_out.BlendState.AlphaToCoverageEnable = pipeline_desc.BlendState.AlphaToCoverageEnable;
	desc_out.BlendState.IndependentBlendEnable = pipeline_desc.BlendState.IndependentBlendEnable;
	for (int32_t render_target_index = 0; render_target_index < pipeline_desc.BlendState.RenderTarget.size(); ++
	     render_target_index)
	{
		desc_out.BlendState.RenderTarget[render_target_index] = GetRenderTargetBlendDesc(
			pipeline_desc.BlendState.RenderTarget[render_target_index]);
		desc_out.RTVFormats[render_target_index] = GetGraphicFormat(pass_desc.mColors[render_target_index].mFormat);
	}
	desc_out.DepthStencilState.DepthEnable = pipeline_desc.DepthStencilState.DepthEnable;
	desc_out.DepthStencilState.DepthWriteMask = GetDepthMask(pipeline_desc.DepthStencilState.DepthWrite);
	desc_out.DepthStencilState.DepthFunc = GetCompareFunc(pipeline_desc.DepthStencilState.DepthFunc);
	desc_out.DepthStencilState.StencilEnable = pipeline_desc.DepthStencilState.StencilEnable;
	desc_out.DepthStencilState.StencilReadMask = pipeline_desc.DepthStencilState.StencilReadMask;
	desc_out.DepthStencilState.StencilWriteMask = pipeline_desc.DepthStencilState.StencilWriteMask;
	desc_out.DepthStencilState.FrontFace.StencilFunc = GetCompareFunc(
		pipeline_desc.DepthStencilState.FrontFace.StencilFunc);
	desc_out.DepthStencilState.FrontFace.StencilDepthFailOp = GetStencilOption(
		pipeline_desc.DepthStencilState.FrontFace.StencilDepthFailOp);
	desc_out.DepthStencilState.FrontFace.StencilPassOp = GetStencilOption(
		pipeline_desc.DepthStencilState.FrontFace.StencilPassOp);
	desc_out.DepthStencilState.FrontFace.StencilFailOp = GetStencilOption(
		pipeline_desc.DepthStencilState.FrontFace.StencilFailOp);
	desc_out.DepthStencilState.BackFace.StencilFunc = GetCompareFunc(
		pipeline_desc.DepthStencilState.BackFace.StencilFunc);
	desc_out.DepthStencilState.BackFace.StencilDepthFailOp = GetStencilOption(
		pipeline_desc.DepthStencilState.BackFace.StencilDepthFailOp);
	desc_out.DepthStencilState.BackFace.StencilPassOp = GetStencilOption(
		pipeline_desc.DepthStencilState.BackFace.StencilPassOp);
	desc_out.DepthStencilState.BackFace.StencilFailOp = GetStencilOption(
		pipeline_desc.DepthStencilState.BackFace.StencilFailOp);
	desc_out.SampleMask = pipeline_desc.SampleMask;
	desc_out.PrimitiveTopologyType = GetTypologyType(pipeline_desc.PrimitiveTopologyType);
	desc_out.NumRenderTargets = pass_desc.mColors.size();
	for (int32_t rtv_index = 0; rtv_index < pass_desc.mColors.size(); ++rtv_index)
	{
		desc_out.RTVFormats[rtv_index] = GetGraphicFormat(pass_desc.mColors[rtv_index].mFormat);
	}
	desc_out.DSVFormat = GetGraphicFormat(pass_desc.mDepths[0].mDepthStencilFormat);
	desc_out.SampleDesc.Count = pipeline_desc.SampleDesc.Count;
	desc_out.SampleDesc.Quality = pipeline_desc.SampleDesc.Quality;
	return desc_out;
}

D3D12_FILL_MODE DX12PipelineState::GetFillMode(const RHIRasterizerFillMode& pipeline_fill_mode)
{
	switch (pipeline_fill_mode)
	{
	case RHIRasterizerFillMode::FILL_MODE_SOLID:
		return D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
	case RHIRasterizerFillMode::FILL_MODE_WIREFRAME:
		return D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME;
	default:
		return D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
	}
}

D3D12_CULL_MODE DX12PipelineState::GetCullMode(const RHIRasterizerCullMode& pipeline_cull_mode)
{
	switch (pipeline_cull_mode)
	{
	case RHIRasterizerCullMode::CULL_MODE_BACK:
		return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
	case RHIRasterizerCullMode::CULL_MODE_FRONT:
		return D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
	case RHIRasterizerCullMode::CULL_MODE_NONE:
		return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
	default:
		return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
	}
}

D3D12_DEPTH_WRITE_MASK DX12PipelineState::GetDepthMask(bool depth_mask)
{
	if(depth_mask)
		return D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL;
	else
		return D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ZERO;
}

D3D12_STENCIL_OP DX12PipelineState::GetStencilOption(const RHIStencilOption& stencil_option)
{
	switch (stencil_option)
	{
	case RHIStencilOption::STENCIL_OP_KEEP:
		return D3D12_STENCIL_OP_KEEP;
	case RHIStencilOption::STENCIL_OP_ZERO:
		return D3D12_STENCIL_OP_ZERO;
	case RHIStencilOption::STENCIL_OP_REPLACE:
		return D3D12_STENCIL_OP_REPLACE;
	case RHIStencilOption::STENCIL_OP_INCR_SAT:
		return D3D12_STENCIL_OP_INCR_SAT;
	case RHIStencilOption::STENCIL_OP_DECR_SAT:
		return D3D12_STENCIL_OP_DECR_SAT;
	case RHIStencilOption::STENCIL_OP_INVERT:
		return D3D12_STENCIL_OP_INVERT;
	case RHIStencilOption::STENCIL_OP_INCR:
		return D3D12_STENCIL_OP_INCR;
	case RHIStencilOption::STENCIL_OP_DECR:
		return D3D12_STENCIL_OP_DECR;
	default:
		return D3D12_STENCIL_OP_KEEP;
	}
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE DX12PipelineState::GetTypologyType(
	const RHIPrimitiveTopologyType& typology_type)
{
	switch (typology_type)
	{
	case RHIPrimitiveTopologyType::Undefined:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	case RHIPrimitiveTopologyType::Point:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	case RHIPrimitiveTopologyType::Line:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	case RHIPrimitiveTopologyType::Triangle:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	case RHIPrimitiveTopologyType::Patch:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	default:
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
}

D3D12_RENDER_TARGET_BLEND_DESC DX12PipelineState::GetRenderTargetBlendDesc(
	const RHIBlendStateTargetDesc& render_blend_desc)
{
	D3D12_RENDER_TARGET_BLEND_DESC out_desc = {};
	out_desc.BlendEnable = render_blend_desc.BlendEnable;
	out_desc.LogicOpEnable = render_blend_desc.LogicOpEnable;
	out_desc.SrcBlend = GetRenderBlend(render_blend_desc.SrcBlend);
	out_desc.DestBlend = GetRenderBlend(render_blend_desc.DestBlend);
	out_desc.BlendOp = GetRenderBlendOption(render_blend_desc.BlendOp);
	out_desc.SrcBlendAlpha = GetRenderBlend(render_blend_desc.SrcBlendAlpha);
	out_desc.DestBlendAlpha = GetRenderBlend(render_blend_desc.DestBlendAlpha);
	out_desc.BlendOpAlpha = GetRenderBlendOption(render_blend_desc.BlendOpAlpha);
	out_desc.LogicOp = GetRenderBlendLogicOption(render_blend_desc.LogicOp);
	out_desc.RenderTargetWriteMask = render_blend_desc.RenderTargetWriteMask;
	return out_desc;
}

D3D12_BLEND DX12PipelineState::GetRenderBlend(const RHIRenderBlend& render_blend)
{
	switch (render_blend)
	{
	case BLEND_ZERO:
		return D3D12_BLEND_ZERO;
	case RHIRenderBlend::BLEND_ONE:
		return D3D12_BLEND_ONE;
	case RHIRenderBlend::BLEND_SRC_COLOR:
		return D3D12_BLEND_SRC_COLOR;
	case RHIRenderBlend::BLEND_INV_SRC_COLOR:
		return D3D12_BLEND_INV_SRC_COLOR;
	case RHIRenderBlend::BLEND_SRC_ALPHA:
		return D3D12_BLEND_SRC_ALPHA;
	case RHIRenderBlend::BLEND_INV_SRC_ALPHA:
		return D3D12_BLEND_INV_SRC_ALPHA;
	case RHIRenderBlend::BLEND_DEST_ALPHA:
		return D3D12_BLEND_DEST_ALPHA;
	case RHIRenderBlend::BLEND_INV_DEST_ALPHA:
		return D3D12_BLEND_INV_DEST_ALPHA;
	case RHIRenderBlend::BLEND_DEST_COLOR:
		return D3D12_BLEND_DEST_COLOR;
	case RHIRenderBlend::BLEND_INV_DEST_COLOR:
		return D3D12_BLEND_INV_DEST_COLOR;
	case RHIRenderBlend::BLEND_SRC_ALPHA_SAT:
		return D3D12_BLEND_SRC_ALPHA_SAT;
	case RHIRenderBlend::BLEND_BLEND_FACTOR:
		return D3D12_BLEND_BLEND_FACTOR;
	case RHIRenderBlend::BLEND_INV_BLEND_FACTOR:
		return D3D12_BLEND_INV_BLEND_FACTOR;
	case RHIRenderBlend::BLEND_SRC1_COLOR:
		return D3D12_BLEND_SRC1_COLOR;
	case RHIRenderBlend::BLEND_INV_SRC1_COLOR:
		return D3D12_BLEND_INV_SRC1_COLOR;
	case RHIRenderBlend::BLEND_SRC1_ALPHA:
		return D3D12_BLEND_SRC1_ALPHA;
	case RHIRenderBlend::BLEND_INV_SRC1_ALPHA:
		return D3D12_BLEND_INV_SRC1_ALPHA;
	default:
		return D3D12_BLEND_ZERO;
	};
}

D3D12_BLEND_OP DX12PipelineState::GetRenderBlendOption(const RHIBlendOption& render_blend_option)
{
	switch (render_blend_option)
	{
	case RHIBlendOption::BLEND_OP_ADD:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	case RHIBlendOption::BLEND_OP_MAX:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_MAX;
	case RHIBlendOption::BLEND_OP_MIN:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_MIN;
	case RHIBlendOption::BLEND_OP_REV_SUBTRACT:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_REV_SUBTRACT;
	case RHIBlendOption::BLEND_OP_SUBTRACT:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_SUBTRACT;
	default:
		return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
	}
}

D3D12_LOGIC_OP DX12PipelineState::GetRenderBlendLogicOption(const RHIBlendLogicOption& render_blend_logic_option)
{
	switch (render_blend_logic_option)
	{
	case RHIBlendLogicOption::LOGIC_OP_CLEAR:
		return D3D12_LOGIC_OP_CLEAR;
	case RHIBlendLogicOption::LOGIC_OP_SET:
		return D3D12_LOGIC_OP_SET;
	case RHIBlendLogicOption::LOGIC_OP_COPY:
		return D3D12_LOGIC_OP_COPY;
	case RHIBlendLogicOption::LOGIC_OP_COPY_INVERTED:
		return D3D12_LOGIC_OP_COPY_INVERTED;
	case RHIBlendLogicOption::LOGIC_OP_NOOP:
		return D3D12_LOGIC_OP_NOOP;
	case RHIBlendLogicOption::LOGIC_OP_INVERT:
		return D3D12_LOGIC_OP_INVERT;
	case RHIBlendLogicOption::LOGIC_OP_AND:
		return D3D12_LOGIC_OP_AND;
	case RHIBlendLogicOption::LOGIC_OP_NAND:
		return D3D12_LOGIC_OP_NAND;
	case RHIBlendLogicOption::LOGIC_OP_OR:
		return D3D12_LOGIC_OP_OR;
	case RHIBlendLogicOption::LOGIC_OP_NOR:
		return D3D12_LOGIC_OP_NOR;
	case RHIBlendLogicOption::LOGIC_OP_XOR:
		return D3D12_LOGIC_OP_XOR;
	case RHIBlendLogicOption::LOGIC_OP_EQUIV:
		return D3D12_LOGIC_OP_EQUIV;
	case RHIBlendLogicOption::LOGIC_OP_AND_REVERSE:
		return D3D12_LOGIC_OP_AND_REVERSE;
	case RHIBlendLogicOption::LOGIC_OP_AND_INVERTED:
		return D3D12_LOGIC_OP_AND_INVERTED;
	case RHIBlendLogicOption::LOGIC_OP_OR_REVERSE:
		return D3D12_LOGIC_OP_OR_REVERSE;
	case RHIBlendLogicOption::LOGIC_OP_OR_INVERTED:
		return D3D12_LOGIC_OP_OR_INVERTED;
	default:
		return D3D12_LOGIC_OP_OR_INVERTED;
	}
}


}

