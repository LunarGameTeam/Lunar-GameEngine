#include"LDirectX12Pipeline.h"
using namespace luna;
using Microsoft::WRL::ComPtr;
BIND_REFLECTION_CLASS_STATIC_ARRAY(
	PipelineStateDescGraphic,
	reflect_data.pso_desc.RTVFormats,
	reflect_data.pso_desc.NumRenderTargets,
	reflect_data.pso_desc.BlendState.RenderTarget,
	reflect_data.pso_desc.NumRenderTargets
);
void LDx12GraphicResourcePipelineState::CheckIfLoadingStateChanged(LLoadState& m_object_load_state)
{
	m_object_load_state = LLoadState::LOAD_STATE_FINISHED;
};
LResult LDx12GraphicResourcePipelineState::GetVertexShaderInputLayoutDesc(ID3D12ShaderReflection* t_ShaderReflection, LVector<D3D12_INPUT_ELEMENT_DESC>& t_InputElementDescVec)
{
	HRESULT hr;
	LResult check_error;
	/*
	http://www.cnblogs.com/macom/archive/2013/10/30/3396419.html
	*/
	D3D12_SHADER_DESC t_ShaderDesc;
	hr = t_ShaderReflection->GetDesc(&t_ShaderDesc);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "get desc of shader reflect error", error_message);

		return error_message;
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
	return g_Succeed;
}
luna::LResult LDx12GraphicResourcePipelineState::InitResorceByDesc(const  LunarPipelineStateDesc& resource_desc)
{
	LResult check_error;
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
		return error_message;
	}
	//根据格式文件创建管线资源
	if (resource_desc.pipeline_type == LunarGraphicPipeLineType::PIPELINE_GRAPHIC3D)
	{
		PipelineStateDescGraphic graphic_pipeline_desc = resource_desc.pipeline_data_graphic;
		//根据rootsignature的名称获取rootsignature信息
		Json::Value root_signature_desc;
		auto if_succeed = PancyJsonTool::GetInstance()->LoadJsonFile(graphic_pipeline_desc.root_signature_file.c_str(), root_signature_desc);
		if (!if_succeed.m_IsOK)
		{
			return if_succeed;
		}
		LDx12GraphicResourceRootSignature* now_root_signature_data = LCreateAssetByJson<LDx12GraphicResourceRootSignature>(graphic_pipeline_desc.root_signature_file, root_signature_desc, true);
		if (now_root_signature_data == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "Directx device is broken ", error_message);
			return error_message;
		}
		root_signature_use = now_root_signature_data;
		//读取pipeline的信息
		D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc_out = graphic_pipeline_desc.pso_desc;
		desc_out = graphic_pipeline_desc.pso_desc;
		desc_out.pRootSignature = now_root_signature_data->GetRootSignature();
		//获取着色器
		LString shader_file_name[] =
		{
			graphic_pipeline_desc.vertex_shader_file,
			graphic_pipeline_desc.pixel_shader_file,
			graphic_pipeline_desc.geometry_shader_file,
			graphic_pipeline_desc.hull_shader_file,
			graphic_pipeline_desc.domin_shader_file,
		};
		for (int i = 0; i < 5; ++i)
		{
			if (shader_file_name[i] != "0")
			{
				Json::Value shader_desc;
				auto if_succeed = PancyJsonTool::GetInstance()->LoadJsonFile(shader_file_name[i].c_str(), shader_desc);
				if (!if_succeed.m_IsOK)
				{
					return if_succeed;
				}
				LDx12GraphicResourceShaderBlob* now_shader_data = LCreateAssetByJson<LDx12GraphicResourceShaderBlob>(shader_file_name[i], shader_desc, true);
				//填充shader信息
				switch (i)
				{
				case 0:
				{
					desc_out.VS = CD3DX12_SHADER_BYTECODE(now_shader_data->GetShader());
					m_shader_use.emplace(luna::LunarGraphicShaderType::SHADER_VERTEX, this);
					m_shader_use.find(luna::LunarGraphicShaderType::SHADER_VERTEX)->second = now_shader_data;
					break;
				}
				case 1:
					desc_out.PS = CD3DX12_SHADER_BYTECODE(now_shader_data->GetShader());
					m_shader_use.emplace(luna::LunarGraphicShaderType::SHADER_PIXEL, this);
					m_shader_use.find(luna::LunarGraphicShaderType::SHADER_PIXEL)->second = now_shader_data;
					break;
				case 2:
					desc_out.GS = CD3DX12_SHADER_BYTECODE(now_shader_data->GetShader());
					m_shader_use.emplace(luna::LunarGraphicShaderType::SHADER_GEOMETRY, this);
					m_shader_use.find(luna::LunarGraphicShaderType::SHADER_GEOMETRY)->second = now_shader_data;
					break;
				case 3:
					desc_out.HS = CD3DX12_SHADER_BYTECODE(now_shader_data->GetShader());
					m_shader_use.emplace(luna::LunarGraphicShaderType::SHADER_HULL, this);
					m_shader_use.find(luna::LunarGraphicShaderType::SHADER_HULL)->second = now_shader_data;
					break;
				case 4:
					desc_out.DS = CD3DX12_SHADER_BYTECODE(now_shader_data->GetShader());
					m_shader_use.emplace(luna::LunarGraphicShaderType::SHADER_DOMIN, this);
					m_shader_use.find(luna::LunarGraphicShaderType::SHADER_DOMIN)->second = now_shader_data;
					break;
				default:
					break;
				}
			}
		}
		//根据顶点着色器获取顶点输入格式

		ID3D12ShaderReflection* vertex_shader_reflection = m_shader_use.find(luna::LunarGraphicShaderType::SHADER_VERTEX)->second.Get()->GetShaderReflection();
		LVector<D3D12_INPUT_ELEMENT_DESC> input_desc_array;
		check_error = GetVertexShaderInputLayoutDesc(vertex_shader_reflection, input_desc_array);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		D3D12_INPUT_ELEMENT_DESC* vertex_desc = new D3D12_INPUT_ELEMENT_DESC[input_desc_array.size()];
		for (int32_t element_index = 0; element_index < input_desc_array.size(); ++element_index)
		{
			vertex_desc[element_index] = input_desc_array[element_index];
		}
		desc_out.InputLayout.pInputElementDescs = vertex_desc;
		desc_out.InputLayout.NumElements = static_cast<UINT>(input_desc_array.size());
		//参数填写完毕后，开始创建图形管线
		HRESULT hr = directx_device->CreateGraphicsPipelineState(&desc_out, IID_PPV_ARGS(&m_pipeline_data));
		if (FAILED(hr))
		{
			LResult error_message;
			LunarDebugLogError(hr, "Create PSO error name " + GetObjectName(), error_message);
			return error_message;
		}
	}
	else if (resource_desc.pipeline_type == LunarGraphicPipeLineType::PIPELINE_COMPUTE)
	{
		PipelineStateDescCompute compute_pipeline_desc = resource_desc.pipeline_data_compute;
		//根据读取的信息建立管线数据
		Json::Value root_signature_desc;
		auto if_succeed = PancyJsonTool::GetInstance()->LoadJsonFile(compute_pipeline_desc.root_signature_file.c_str(), root_signature_desc);
		if (!if_succeed.m_IsOK)
		{
			return if_succeed;
		}
		LDx12GraphicResourceRootSignature* now_root_signature_data = LCreateAssetByJson<LDx12GraphicResourceRootSignature>(compute_pipeline_desc.root_signature_file, root_signature_desc, true);
		if (now_root_signature_data == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "Directx device is broken ", error_message);
			return error_message;
		}
		root_signature_use = now_root_signature_data;
		D3D12_COMPUTE_PIPELINE_STATE_DESC  desc_out = {};
		desc_out.pRootSignature = now_root_signature_data->GetRootSignature();
		//计算着色器pipeline
		if (compute_pipeline_desc.compute_shader_file != "0")
		{
			Json::Value shader_desc;
			auto if_succeed = PancyJsonTool::GetInstance()->LoadJsonFile(compute_pipeline_desc.compute_shader_file.c_str(), shader_desc);
			if (!if_succeed.m_IsOK)
			{
				return if_succeed;
			}
			LDx12GraphicResourceShaderBlob* now_shader_data = LCreateAssetByJson<LDx12GraphicResourceShaderBlob>(compute_pipeline_desc.compute_shader_file, shader_desc, true);
			//填充shader信息
			desc_out.CS = CD3DX12_SHADER_BYTECODE(now_shader_data->GetShader());
			m_shader_use.emplace(luna::LunarGraphicShaderType::SHADER_COMPUTE, this);
			m_shader_use.find(luna::LunarGraphicShaderType::SHADER_COMPUTE)->second = now_shader_data;
			//创建资源
			HRESULT hr = directx_device->CreateComputePipelineState(&desc_out, IID_PPV_ARGS(&m_pipeline_data));
			if (FAILED(hr))
			{
				LResult error_message;
				LunarDebugLogError(hr, "Create PSO error name " + GetObjectName(), error_message);
				return error_message;
			}
		}
	}
	else
	{
		LResult error_message;
		LunarDebugLogError(0, "unrecognized pipeline state " + GetObjectName(), error_message);
		return error_message;
	}
	return luna::g_Succeed;
}
void luna::InitPipelineJsonReflect()
{
	InitNewStructToReflection(DescriptorTypeDesc);
	InitNewStructToReflection(PipelineStateDescCompute);
	InitNewStructToReflection(D3D12_RENDER_TARGET_BLEND_DESC);
	InitNewStructToReflection(PipelineStateDescGraphic);
	//PSO格式
	InitNewEnumValue(PIPELINE_GRAPHIC3D);
	InitNewEnumValue(PIPELINE_COMPUTE);
	//几何体填充格式
	InitNewEnumValue(D3D12_FILL_MODE_WIREFRAME);
	InitNewEnumValue(D3D12_FILL_MODE_SOLID);
	//几何体消隐格式
	InitNewEnumValue(D3D12_CULL_MODE_NONE);
	InitNewEnumValue(D3D12_CULL_MODE_FRONT);
	InitNewEnumValue(D3D12_CULL_MODE_BACK);
	//alpha混合系数
	InitNewEnumValue(D3D12_BLEND_ZERO);
	InitNewEnumValue(D3D12_BLEND_ONE);
	InitNewEnumValue(D3D12_BLEND_SRC_COLOR);
	InitNewEnumValue(D3D12_BLEND_INV_SRC_COLOR);
	InitNewEnumValue(D3D12_BLEND_SRC_ALPHA);
	InitNewEnumValue(D3D12_BLEND_INV_SRC_ALPHA);
	InitNewEnumValue(D3D12_BLEND_DEST_ALPHA);
	InitNewEnumValue(D3D12_BLEND_INV_DEST_ALPHA);
	InitNewEnumValue(D3D12_BLEND_DEST_COLOR);
	InitNewEnumValue(D3D12_BLEND_INV_DEST_COLOR);
	InitNewEnumValue(D3D12_BLEND_SRC_ALPHA_SAT);
	InitNewEnumValue(D3D12_BLEND_BLEND_FACTOR);
	InitNewEnumValue(D3D12_BLEND_INV_BLEND_FACTOR);
	InitNewEnumValue(D3D12_BLEND_SRC1_COLOR);
	InitNewEnumValue(D3D12_BLEND_INV_SRC1_COLOR);
	InitNewEnumValue(D3D12_BLEND_SRC1_ALPHA);
	InitNewEnumValue(D3D12_BLEND_INV_SRC1_ALPHA);
	//alpha混合操作
	InitNewEnumValue(D3D12_BLEND_OP_ADD);
	InitNewEnumValue(D3D12_BLEND_OP_SUBTRACT);
	InitNewEnumValue(D3D12_BLEND_OP_REV_SUBTRACT);
	InitNewEnumValue(D3D12_BLEND_OP_MIN);
	InitNewEnumValue(D3D12_BLEND_OP_MAX);
	//alpha混合logic操作
	InitNewEnumValue(D3D12_LOGIC_OP_CLEAR);
	InitNewEnumValue(D3D12_LOGIC_OP_SET);
	InitNewEnumValue(D3D12_LOGIC_OP_COPY);
	InitNewEnumValue(D3D12_LOGIC_OP_COPY_INVERTED);
	InitNewEnumValue(D3D12_LOGIC_OP_NOOP);
	InitNewEnumValue(D3D12_LOGIC_OP_INVERT);
	InitNewEnumValue(D3D12_LOGIC_OP_AND);
	InitNewEnumValue(D3D12_LOGIC_OP_NAND);
	InitNewEnumValue(D3D12_LOGIC_OP_OR);
	InitNewEnumValue(D3D12_LOGIC_OP_NOR);
	InitNewEnumValue(D3D12_LOGIC_OP_XOR);
	InitNewEnumValue(D3D12_LOGIC_OP_EQUIV);
	InitNewEnumValue(D3D12_LOGIC_OP_AND_REVERSE);
	InitNewEnumValue(D3D12_LOGIC_OP_AND_INVERTED);
	InitNewEnumValue(D3D12_LOGIC_OP_OR_REVERSE);
	InitNewEnumValue(D3D12_LOGIC_OP_OR_INVERTED);
	//alpha混合目标掩码
	InitNewEnumValue(D3D12_COLOR_WRITE_ENABLE_RED);
	InitNewEnumValue(D3D12_COLOR_WRITE_ENABLE_GREEN);
	InitNewEnumValue(D3D12_COLOR_WRITE_ENABLE_BLUE);
	InitNewEnumValue(D3D12_COLOR_WRITE_ENABLE_ALPHA);
	InitNewEnumValue(D3D12_COLOR_WRITE_ENABLE_ALL);
	//深度缓冲区写掩码
	InitNewEnumValue(D3D12_DEPTH_WRITE_MASK_ZERO);
	InitNewEnumValue(D3D12_DEPTH_WRITE_MASK_ALL);
	//深度缓冲区比较函数
	InitNewEnumValue(D3D12_COMPARISON_FUNC_NEVER);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_LESS);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_LESS_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_GREATER);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_NOT_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_GREATER_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_ALWAYS);
	//模板缓冲区操作
	InitNewEnumValue(D3D12_STENCIL_OP_KEEP);
	InitNewEnumValue(D3D12_STENCIL_OP_ZERO);
	InitNewEnumValue(D3D12_STENCIL_OP_REPLACE);
	InitNewEnumValue(D3D12_STENCIL_OP_INCR_SAT);
	InitNewEnumValue(D3D12_STENCIL_OP_DECR_SAT);
	InitNewEnumValue(D3D12_STENCIL_OP_INVERT);
	InitNewEnumValue(D3D12_STENCIL_OP_INCR);
	InitNewEnumValue(D3D12_STENCIL_OP_DECR);
	//渲染图元格式
	InitNewEnumValue(D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED);
	InitNewEnumValue(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
	InitNewEnumValue(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	InitNewEnumValue(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	InitNewEnumValue(D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH);
	//Descriptor格式
	InitNewEnumValue(CbufferPrivate);
	InitNewEnumValue(CbufferGlobel);
	InitNewEnumValue(SRVGlobel);
	InitNewEnumValue(SRVPrivate);
	InitNewEnumValue(SRVBindless);
}