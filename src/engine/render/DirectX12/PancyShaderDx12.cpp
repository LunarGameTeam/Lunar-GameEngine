#include"PancyShaderDx12.h"
using namespace LunarEngine;
BIND_REFLECTION_CLASS_STATIC_ARRAY(
	PipelineStateDescGraphic,
	reflect_data.pso_desc.RTVFormats,
	reflect_data.pso_desc.NumRenderTargets,
	reflect_data.pso_desc.BlendState.RenderTarget,
	reflect_data.pso_desc.NumRenderTargets
);
//输入顶点格式
InputLayoutDesc::InputLayoutDesc()
{
}
InputLayoutDesc::~InputLayoutDesc()
{
	for (auto now_vertex_desc = vertex_buffer_desc_map.begin(); now_vertex_desc != vertex_buffer_desc_map.end(); ++now_vertex_desc)
	{
		delete now_vertex_desc->second.inputElementDescs;
		now_vertex_desc->second.inputElementDescs = NULL;
	}
	vertex_buffer_desc_map.clear();
}
void InputLayoutDesc::AddVertexDesc(std::string vertex_desc_name_in, std::vector<D3D12_INPUT_ELEMENT_DESC> input_element_desc_list)
{
	PancyVertexBufferDesc new_vertex_desc;
	new_vertex_desc.vertex_desc_name = vertex_desc_name_in;
	new_vertex_desc.input_element_num = input_element_desc_list.size();
	new_vertex_desc.inputElementDescs = new D3D12_INPUT_ELEMENT_DESC[input_element_desc_list.size()];
	uint32_t count_element = 0;
	for (auto now_element = input_element_desc_list.begin(); now_element != input_element_desc_list.end(); ++now_element)
	{
		new_vertex_desc.inputElementDescs[count_element] = *now_element;
		count_element += 1;
	}
	vertex_buffer_desc_map.insert(std::pair<std::string, PancyVertexBufferDesc>(vertex_desc_name_in, new_vertex_desc));
}
//shader编译器
PancyShaderBasic::PancyShaderBasic(
	const std::string& shader_file_in,
	const std::string& main_func_name,
	const std::string& shader_type
)
{
	shader_file_name = shader_file_in;
	shader_entry_point_name = main_func_name;
	shader_type_name = shader_type;
}
LResult PancyShaderBasic::Create()
{
	//| D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#else
	UINT compileFlags = 0 | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#endif
	//编译shader文件
	ID3D10Blob* error_message = NULL;
	HRESULT hr = D3DCompileFromFile(shader_file_name.GetStdUnicodeString().c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shader_entry_point_name.str().c_str(), shader_type_name.str().c_str(), compileFlags, 0, &shader_memory_pointer, &error_message);
	if (FAILED(hr))
	{

		char data[1000];
		if (error_message != NULL)
		{
			memcpy(data, error_message->GetBufferPointer(), error_message->GetBufferSize());
		}
		LResult error_message;
		LunarDebugLogError(hr, "Compile shader : " + shader_file_name.str() + ":: " + shader_entry_point_name.str() + " error:" + data, error_message);

		return error_message;
	}
	//获取shader反射
	hr = D3DReflect(shader_memory_pointer->GetBufferPointer(), shader_memory_pointer->GetBufferSize(), IID_ID3D12ShaderReflection, &shader_reflection);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "get shader reflect message : " + shader_file_name.str() + ":: " + shader_entry_point_name.str() + " error", error_message);

		return error_message;
	}
	//获取输入格式
	D3D12_SHADER_DESC shader_desc;
	shader_reflection->GetDesc(&shader_desc);
	for (UINT i = 0; i < shader_desc.InputParameters; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC now_param_desc;
		shader_reflection->GetInputParameterDesc(i, &now_param_desc);
	}
	//获取常量缓冲区
	/*
	for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i)
	{
		auto now_constant_buffer = shader_reflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC buffer_shader;
		now_constant_buffer->GetDesc(&buffer_shader);
		int a = 0;
	}

	for (UINT i = 0; i < shader_desc.BoundResources; ++i)
	{
		D3D12_SHADER_INPUT_BIND_DESC now_bind;
		shader_reflection->GetResourceBindingDesc(i, &now_bind);
		int a = 0;
	}
	*/
	return g_Succeed;
}
//shader管理器
PancyShaderControl::PancyShaderControl()
{

}
LResult PancyShaderControl::LoadShader(const std::string& shader_file, const std::string& shader_main_func, const std::string& shader_type)
{
	std::string shader_final_name = shader_file + "::" + shader_main_func;
	auto check_data = shader_list.find(shader_final_name);
	if (check_data != shader_list.end())
	{
		LResult error_message;
		LunarDebugLogWarning(E_FAIL, "the shader " + shader_final_name + " had been insert to map before", error_message);

		return error_message;
	}
	PancyShaderBasic* new_shader = new PancyShaderBasic(shader_file, shader_main_func, shader_type);
	auto check_error = new_shader->Create();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	shader_list.insert(std::pair<std::string, PancyShaderBasic*>(shader_final_name, new_shader));
	return g_Succeed;
}
LResult PancyShaderControl::GetShaderReflection(const std::string& shader_file, const std::string& shader_main_func, const std::string& shader_type, ComPtr<ID3D12ShaderReflection>* res_data)
{
	LResult check_error;
	std::string shader_final_name = shader_file + "::" + shader_main_func;
	auto shader_data = shader_list.find(shader_final_name);
	if (shader_data == shader_list.end())
	{
		//shader尚未加载，重新从文件中加载shader
		check_error = LoadShader(shader_file, shader_main_func, shader_type);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	HRESULT hr = shader_data->second->GetShaderReflect().As(res_data);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not copy shader reflection com_ptr of shader " + shader_final_name, error_message);

		return error_message;
	}
	return g_Succeed;
}
LResult PancyShaderControl::GetShaderData(const std::string& shader_file, const std::string& shader_main_func, const std::string& shader_type, ComPtr<ID3DBlob>* res_data)
{
	LResult check_error;
	std::string shader_final_name = shader_file + "::" + shader_main_func;
	auto shader_data = shader_list.find(shader_final_name);
	if (shader_data == shader_list.end())
	{
		//shader尚未加载，重新从文件中加载shader
		check_error = LoadShader(shader_file, shader_main_func, shader_type);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		shader_data = shader_list.find(shader_final_name);
	}
	HRESULT hr = shader_data->second->GetShader().As(res_data);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not copy shader data com_ptr of shader " + shader_final_name, error_message);

		return error_message;
	}
	return g_Succeed;
}
PancyShaderControl::~PancyShaderControl()
{
	for (auto data = shader_list.begin(); data != shader_list.end(); ++data)
	{
		delete data->second;
	}
	shader_list.clear();
}
//rootsignature
PancyRootSignature::PancyRootSignature(const std::string& file_name)
{
	root_signature_name = file_name;
}
LResult PancyRootSignature::Create()
{
	LResult check_error;
	//借助json反射将文件中的格式读取到类中
	RootSignatureDesc now_root_signature_desc;
	check_error = LSrtructReflectControl::GetInstance()->DeserializeFromJsonFile(now_root_signature_desc, root_signature_name.str());
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//创建rootsignature
	D3D12_STATIC_SAMPLER_DESC* data_sampledesc;
	CD3DX12_DESCRIPTOR_RANGE1* ranges;
	CD3DX12_ROOT_PARAMETER1* rootParameters;
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc_out;
	auto num_parameter = now_root_signature_desc.root_parameter_data.size();
	if (num_parameter > 0)
	{
		ranges = new CD3DX12_DESCRIPTOR_RANGE1[num_parameter];
		rootParameters = new CD3DX12_ROOT_PARAMETER1[num_parameter];
	}
	else
	{
		ranges = NULL;
		rootParameters = NULL;
	}
	int32_t all_descriptor_num = 0;
	for (int i = 0; i < num_parameter; ++i)
	{
		//获取需要range_type(cbv/srv/......)
		D3D12_DESCRIPTOR_RANGE_TYPE range_type = now_root_signature_desc.root_parameter_data[i].range_type;
		//获取需要创建的descriptor(cbv/srv/......)的数量
		int32_t descriptor_num = now_root_signature_desc.root_parameter_data[i].num_descriptors;
		//获取这组descriptor变量对应的首个寄存器的地址
		int32_t base_registor = now_root_signature_desc.root_parameter_data[i].base_shader_register;
		//获取这组descriptor变量对应的寄存器域的地址
		int32_t base_registor_space = now_root_signature_desc.root_parameter_data[i].register_space;
		//获取这组descriptor变量对应的格式
		D3D12_DESCRIPTOR_RANGE_FLAGS flag_type = now_root_signature_desc.root_parameter_data[i].flags;
		//注册descriptor range
		ranges[i].Init(range_type, descriptor_num, base_registor, base_registor_space, flag_type);
		//获取该descriptor range需要被创建的次数
		int32_t descriptor_range_num = now_root_signature_desc.root_parameter_data[i].num_descriptor_ranges;
		//获取shader的访问权限
		D3D12_SHADER_VISIBILITY shader_visibility_type = now_root_signature_desc.root_parameter_data[i].shader_visibility;
		//注册rootsignature格式
		rootParameters[i].InitAsDescriptorTable(descriptor_range_num, &ranges[i], shader_visibility_type);
		all_descriptor_num += descriptor_num;
	}
	LunarObjectID num_static_sampler;
	//获取每个静态采样器
	num_static_sampler = static_cast<LunarObjectID>(now_root_signature_desc.static_sampler_data.size());
	if (num_static_sampler > 0)
	{
		data_sampledesc = new D3D12_STATIC_SAMPLER_DESC[num_static_sampler];
	}
	else
	{
		data_sampledesc = NULL;
	}
	for (LunarObjectID i = 0; i < num_static_sampler; ++i)
	{
		data_sampledesc[i] = now_root_signature_desc.static_sampler_data[i];
	}
	//获取rootsignature格式
	D3D12_ROOT_SIGNATURE_FLAGS root_signature = now_root_signature_desc.root_signature_flags;
	desc_out.Init_1_1(static_cast<UINT>(num_parameter), rootParameters, num_static_sampler, data_sampledesc, root_signature);
	//创建rootsignature
	check_error = BuildResource(desc_out);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//删除临时资源
	if (ranges != NULL)
	{
		delete[] ranges;
		ranges = NULL;
	}
	if (rootParameters != NULL)
	{
		delete[] rootParameters;
		rootParameters = NULL;
	}
	if (data_sampledesc != NULL)
	{
		delete[] data_sampledesc;
		data_sampledesc = NULL;
	}
	return g_Succeed;
}
LResult PancyRootSignature::BuildResource(
	const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& rootSignatureDesc
)
{
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	HRESULT hr;
	hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "serial rootsignature " + root_signature_name.str() + " error", error_message);

		return error_message;
	}
	hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature_data));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "Create root signature " + root_signature_name.str() + " failed", error_message);

		return error_message;
	}
	return g_Succeed;
}
std::string PancyRootSignature::GetJsonFileRealName(const std::string& file_name_in)
{
	std::string new_str;
	int st = 0;
	int length = -4;
	for (int i = static_cast<int>(file_name_in.size() - 2); i >= 0; --i)
	{
		if (file_name_in[i - 1] == '\\')
		{
			st = i;
			break;
		}
		else
		{
			length += 1;
		}
	}
	return file_name_in.substr(st, length);
}
//rootsignature管理器
PancyRootSignatureControl::PancyRootSignatureControl()
{
	RootSig_id_self_add = 0;
	AddRootSignatureGlobelVariable();
}
void PancyRootSignatureControl::AddRootSignatureGlobelVariable()
{
	//descriptor range格式
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
	//descriptor flag格式
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	//shader访问权限
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_ALL);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_VERTEX);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_HULL);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_DOMAIN);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_GEOMETRY);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_PIXEL);
	//采样格式
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_ANISOTROPIC);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_ANISOTROPIC);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_ANISOTROPIC);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_ANISOTROPIC);
	//寻址格式
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_WRAP);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_BORDER);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
	//比较函数格式
	InitNewEnumValue(D3D12_COMPARISON_FUNC_NEVER);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_LESS);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_LESS_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_GREATER);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_NOT_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_GREATER_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_ALWAYS);
	//超出边界的采样颜色
	InitNewEnumValue(D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK);
	InitNewEnumValue(D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
	InitNewEnumValue(D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);
	//root signature的访问权限
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_NONE);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT);
}
LResult PancyRootSignatureControl::GetResource(const LunarObjectID& root_signature_id, ID3D12RootSignature** root_signature_res)
{
	LResult check_error;
	auto root_signature_find = root_signature_array.find(root_signature_id);
	if (root_signature_find == root_signature_array.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find rootsignature ID: " + std::to_string(root_signature_id), error_message);

		return error_message;
	}
	root_signature_find->second->GetResource(root_signature_res);
	return g_Succeed;
}
LResult PancyRootSignatureControl::BuildRootSignature(const std::string& rootsig_config_file)
{
	LResult check_error;
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc = {};
	//判断重复
	auto check_data = RootSig_name.find(rootsig_config_file);
	if (check_data != RootSig_name.end())
	{
		LResult error_message;
		LunarDebugLogWarning(E_FAIL, "the root signature " + rootsig_config_file + " had been insert to map before", error_message);

		return error_message;
	}
	//创建RootSignature
	PancyRootSignature* data_root_signature = new PancyRootSignature(rootsig_config_file);
	check_error = data_root_signature->Create();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//根据ID池是否为空来决定新开辟的资源ID号
	LunarObjectID now_next_id;
	if (empty_RootSig_id.size() > 0)
	{
		now_next_id = empty_RootSig_id.front();
		empty_RootSig_id.pop();
	}
	else
	{
		now_next_id = RootSig_id_self_add;
		RootSig_id_self_add += 1;
	}
	//将资源插入到资源表
	RootSig_name.insert(std::pair<std::string, LunarObjectID >(rootsig_config_file, now_next_id));
	RootSig_name_reflect.insert(std::pair<LunarObjectID, std::string>(now_next_id, rootsig_config_file));
	root_signature_array.insert(std::pair<LunarObjectID, PancyRootSignature*>(now_next_id, data_root_signature));
	return g_Succeed;
}
LResult PancyRootSignatureControl::GetRootSignature(const std::string& name_in, LunarObjectID& root_signature_id)
{
	LResult check_error;
	auto root_signature_find = RootSig_name.find(name_in);
	if (root_signature_find == RootSig_name.end())
	{
		//未找到root signature，尝试加载
		check_error = BuildRootSignature(name_in);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		root_signature_find = RootSig_name.find(name_in);
	}
	root_signature_id = root_signature_find->second;
	return g_Succeed;
}
PancyRootSignatureControl::~PancyRootSignatureControl()
{
	for (auto data = root_signature_array.begin(); data != root_signature_array.end(); ++data)
	{
		delete data->second;
	}
	root_signature_array.clear();
}
//pipline state object graph
PancyPiplineStateObjectGraph::PancyPiplineStateObjectGraph(const std::string& pso_name_in)
{
	pipline_type = PSO_TYPE_GRAPHIC;
	pso_name = pso_name_in;
}
/*
EngineFailReason PancyPiplineStateObjectGraph::GetDescriptorHeapUse(std::string &descriptor_heap_name)
{
	EngineFailReason check_error;
	check_error = PancyRootSignatureControl::GetInstance()->GetDescriptorHeapUse(root_signature_ID, descriptor_heap_name);
	if (!check_error.if_succeed)
	{
		return check_error;
	}
	return succeed;
}

EngineFailReason PancyPiplineStateObjectGraph::GetDescriptorDistribute(std::vector<LunarObjectID> &descriptor_distribute)
{
	EngineFailReason check_error;
	check_error = PancyRootSignatureControl::GetInstance()->GetDescriptorDistribute(root_signature_ID, descriptor_distribute);
	if (!check_error.if_succeed)
	{
		return check_error;
	}
	return succeed;
}
EngineFailReason PancyPiplineStateObjectGraph::CheckCbuffer(const std::string &cbuffer_name)
{
	auto cbuffer_data = Cbuffer_map.find(cbuffer_name);
	if (cbuffer_data == Cbuffer_map.end())
	{
		EngineFailReason error_message;
		LunarDebugLogError(E_FAIL, "Could not find Cbuffer: " + cbuffer_name + " in PSO: " + pso_name.GetStdAsciiString(),error_message);

		return error_message;
	}
	return succeed;
}

EngineFailReason PancyPiplineStateObjectGraph::GetCbuffer(const std::string &cbuffer_name, const Json::Value *& CbufferData)
{
	auto cbuffer_data = Cbuffer_map.find(cbuffer_name);
	if (cbuffer_data == Cbuffer_map.end())
	{
		EngineFailReason error_message;
		LunarDebugLogError(E_FAIL, "Could not find Cbuffer: " + cbuffer_name + " in PSO: " + pso_name.GetStdAsciiString(),error_message);

		return error_message;
	}
	CbufferData = &cbuffer_data->second;
	return succeed;
}
*/
LResult PancyPiplineStateObjectGraph::BuildCbufferByName(
	const std::string& cbuffer_name,
	PancyConstantBuffer& cbuffer_data_out
)
{
	auto cbuffer_allocator = Cbuffer_map.find(cbuffer_name);
	if (cbuffer_allocator == Cbuffer_map.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Could not find Cbuffer: " + cbuffer_name + " in PSO: " + pso_name.str(), error_message);

		return error_message;
	}
	auto check_error = cbuffer_allocator->second->BuildNewCbuffer(cbuffer_data_out);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancyPiplineStateObjectGraph::ReleaseCbufferByID(
	const std::string& cbuffer_name,
	const LunarObjectID& buffer_resource_id,
	const LunarObjectID& buffer_offset_id
)
{
	auto cbuffer_allocator = Cbuffer_map.find(cbuffer_name);
	if (cbuffer_allocator == Cbuffer_map.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "Could not find Cbuffer: " + cbuffer_name + " in PSO: " + pso_name.str(), error_message);

		return error_message;
	}
	auto check_error = cbuffer_allocator->second->ReleaseCbuffer(buffer_resource_id, buffer_offset_id);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancyPiplineStateObjectGraph::BuildCbufferByShaderReflect(ComPtr<ID3D12ShaderReflection>& now_shader_reflect)
{
	LResult check_error;
	D3D12_SHADER_DESC shader_desc;
	now_shader_reflect->GetDesc(&shader_desc);
	for (UINT j = 0; j < shader_desc.ConstantBuffers; ++j)
	{
		auto now_constant_buffer = now_shader_reflect->GetConstantBufferByIndex(j);
		D3D12_SHADER_BUFFER_DESC buffer_shader;
		now_constant_buffer->GetDesc(&buffer_shader);
		if (Cbuffer_map.find(buffer_shader.Name) == Cbuffer_map.end())
		{
			//将cbuffer的大小进行256位对齐
			LunarResourceSize alize_cbuffer_size = buffer_shader.Size;
			LunarResourceSize alize_buffer_resource_size = 65536;
			if (alize_cbuffer_size % 256 != 0)
			{
				alize_cbuffer_size = (1 + (alize_cbuffer_size / 256)) * 256;
			}
			//根据cbuffer的大小来决定开辟的buffer资源的大小
			if (alize_cbuffer_size > 256 * 6)
			{
				//大型Cbuffer(可能由instance数据),开辟256k的buffer作为承载buffer
				alize_buffer_resource_size = 256 * 1024;
			}
			else
			{
				//小型cbuffer，开辟64k的buffer作为承载buffer
				alize_buffer_resource_size = 64 * 1024;
			}
			PancyCommonBufferDesc cbuffer_resource_desc;
			cbuffer_resource_desc.buffer_type = Buffer_Constant;
			cbuffer_resource_desc.buffer_res_desc.Alignment = 0;
			cbuffer_resource_desc.buffer_res_desc.DepthOrArraySize = 1;
			cbuffer_resource_desc.buffer_res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			cbuffer_resource_desc.buffer_res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			cbuffer_resource_desc.buffer_res_desc.Height = 1;
			cbuffer_resource_desc.buffer_res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			cbuffer_resource_desc.buffer_res_desc.MipLevels = 1;
			cbuffer_resource_desc.buffer_res_desc.SampleDesc.Count = 1;
			cbuffer_resource_desc.buffer_res_desc.SampleDesc.Quality = 0;
			cbuffer_resource_desc.buffer_res_desc.Width = alize_buffer_resource_size;
			//准备创建一个新的常量缓冲区
			Json::Value new_cbuffer_desc_value;
			//创建资源格式
			PancyJsonTool::GetInstance()->SetJsonValue(new_cbuffer_desc_value, "BufferType", "Buffer_Constant");
			//填充常量缓冲区
			Json::Value cbuffer_value;
			PancyJsonTool::GetInstance()->SetJsonValue(cbuffer_value, "BufferSize", alize_cbuffer_size);
			for (UINT i = 0; i < buffer_shader.Variables; ++i)
			{
				Json::Value cbuffer_variable_value;
				auto shader_variable = now_constant_buffer->GetVariableByIndex(i);
				D3D12_SHADER_VARIABLE_DESC shader_var_desc;
				shader_variable->GetDesc(&shader_var_desc);
				PancyJsonTool::GetInstance()->SetJsonValue(cbuffer_variable_value, "StartOffset", shader_var_desc.StartOffset);
				PancyJsonTool::GetInstance()->SetJsonValue(cbuffer_variable_value, "Size", shader_var_desc.Size);
				PancyJsonTool::GetInstance()->SetJsonValue(cbuffer_variable_value, "Name", shader_var_desc.Name);
				PancyJsonTool::GetInstance()->AddJsonArrayValue(cbuffer_value, "VariableMember", cbuffer_variable_value);
			}
			PancyJsonTool::GetInstance()->SetJsonValue(new_cbuffer_desc_value, "CbufferDesc", cbuffer_value);
			//将当前cbuffer的格式保存在map中
			ConstantBufferAlloctor* new_cbuffer_alloctar = new ConstantBufferAlloctor(
				alize_cbuffer_size,
				buffer_shader.Name,
				pso_name.str(),
				cbuffer_resource_desc,
				new_cbuffer_desc_value
			);
			Cbuffer_map.insert(std::pair<std::string, ConstantBufferAlloctor*>(buffer_shader.Name, new_cbuffer_alloctar));
		}
	}
	return g_Succeed;
}
LResult PancyPiplineStateObjectGraph::ParseDiscriptorDistribution(const std::vector<DescriptorTypeDesc>& descriptor_desc_in)
{
	LResult check_error;
	for (int i = 0; i < descriptor_desc_in.size(); ++i)
	{
		PancyDescriptorPSODescription new_descriptor_root_signature_bind;
		new_descriptor_root_signature_bind.descriptor_name = descriptor_desc_in[i].name;
		new_descriptor_root_signature_bind.rootsignature_slot = i;
		switch (descriptor_desc_in[i].type)
		{
		case CbufferPrivate:
			private_cbuffer.push_back(new_descriptor_root_signature_bind);
			break;
		case CbufferGlobel:
			globel_cbuffer.push_back(new_descriptor_root_signature_bind);
			break;
		case SRVGlobel:
			globel_shader_res.push_back(new_descriptor_root_signature_bind);
			break;
		case SRVPrivate:
			private_shader_res.push_back(new_descriptor_root_signature_bind);
			break;
		case SRVBindless:
			bindless_shader_res.push_back(new_descriptor_root_signature_bind);
			break;
		default:
			break;
		}
	}
	return g_Succeed;
}
LResult PancyPiplineStateObjectGraph::Create()
{
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_out = {};
	std::string file_name = pso_name.str();
	//获取格式
	Json::Value pre_read_jsonRoot;
	auto check_error = PancyJsonTool::GetInstance()->LoadJsonFile(file_name, pre_read_jsonRoot);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	pancy_json_value now_value;
	//先预读取当前pipeline的格式
	check_error = PancyJsonTool::GetInstance()->GetJsonData(file_name, pre_read_jsonRoot, "pipeline_state_type", pancy_json_data_type::json_data_string, now_value);
	std::string enum_data_name = now_value.string_value;
	check_error = LReflectVariableCopySystem::GetInstance()->SetEnumValueToPointer(typeid(PSOType).hash_code(), enum_data_name, typeid(PSOType).hash_code(), &pipline_type);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//根据管线类型的不同决定使用哪种方式读取管线的信息
	if (pipline_type == PSOType::PSO_TYPE_GRAPHIC)
	{
		//先创建一个图形管线的反射
		PipelineStateDescGraphic graphic_pipeline_desc;
		check_error = LSrtructReflectControl::GetInstance()->DeserializeFromJsonFile(graphic_pipeline_desc, file_name);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//根据读取的信息建立管线数据
		check_error = PancyRootSignatureControl::GetInstance()->GetRootSignature(graphic_pipeline_desc.root_signature_file, root_signature_ID);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		ID3D12RootSignature* root_signature_use;
		PancyRootSignatureControl::GetInstance()->GetResource(root_signature_ID, &root_signature_use);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc_out = graphic_pipeline_desc.pso_desc;
		desc_out = graphic_pipeline_desc.pso_desc;
		desc_out.pRootSignature = root_signature_use;
		//获取着色器
		std::string shader_file_name[] =
		{
			graphic_pipeline_desc.vertex_shader_file,
			graphic_pipeline_desc.pixel_shader_file,
			graphic_pipeline_desc.geometry_shader_file,
			graphic_pipeline_desc.hull_shader_file,
			graphic_pipeline_desc.domin_shader_file,
		};
		std::string shader_func_name[] =
		{
			graphic_pipeline_desc.vertex_shader_func,
			graphic_pipeline_desc.pixel_shader_func,
			graphic_pipeline_desc.geometry_shader_func,
			graphic_pipeline_desc.hull_shader_func,
			graphic_pipeline_desc.domin_shader_func,
		};
		std::string shader_version[] =
		{
			"vs_5_1",
			"ps_5_1",
			"gs_5_1",
			"hs_5_1",
			"ds_5_1",
		};
		for (int i = 0; i < 5; ++i)
		{
			if (shader_file_name[i] != "0" && shader_func_name[i] != "0")
			{
				ComPtr<ID3DBlob> shader_data;
				check_error = PancyShaderControl::GetInstance()->GetShaderData(shader_file_name[i], shader_func_name[i], shader_version[i], &shader_data);
				if (!check_error.m_IsOK)
				{
					return check_error;
				}
				//获取shader reflect并存储cbuffer的信息
				ComPtr<ID3D12ShaderReflection> now_shader_reflect;
				check_error = PancyShaderControl::GetInstance()->GetShaderReflection(shader_file_name[i], shader_func_name[i], shader_version[i], &now_shader_reflect);
				if (!check_error.m_IsOK)
				{
					return check_error;
				}
				check_error = BuildCbufferByShaderReflect(now_shader_reflect);
				if (!check_error.m_IsOK)
				{
					return check_error;
				}
				//填充shader信息
				switch (i)
				{
				case 0:
					desc_out.VS = CD3DX12_SHADER_BYTECODE(shader_data.Get());
					break;
				case 1:
					desc_out.PS = CD3DX12_SHADER_BYTECODE(shader_data.Get());
					break;
				case 2:
					desc_out.GS = CD3DX12_SHADER_BYTECODE(shader_data.Get());
					break;
				case 3:
					desc_out.HS = CD3DX12_SHADER_BYTECODE(shader_data.Get());
					break;
				case 4:
					desc_out.DS = CD3DX12_SHADER_BYTECODE(shader_data.Get());
					break;
				default:
					break;
				}
			}
		}
		//根据顶点着色器获取顶点输入格式
		ComPtr<ID3D12ShaderReflection> vertex_shader_reflect;
		check_error = PancyShaderControl::GetInstance()->GetShaderReflection(graphic_pipeline_desc.vertex_shader_file, graphic_pipeline_desc.vertex_shader_func, "vs_5_1", &vertex_shader_reflect);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		if (InputLayoutDesc::GetInstance()->GetVertexDesc(graphic_pipeline_desc.vertex_shader_file + "::" + graphic_pipeline_desc.vertex_shader_func) == NULL)
		{
			//未找到输入缓冲区，添加新的缓冲区
			std::vector<D3D12_INPUT_ELEMENT_DESC> input_desc_array;
			check_error = GetInputDesc(vertex_shader_reflect, input_desc_array);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			InputLayoutDesc::GetInstance()->AddVertexDesc(graphic_pipeline_desc.vertex_shader_file + "::" + graphic_pipeline_desc.vertex_shader_func, input_desc_array);
		}
		auto vertex_desc = InputLayoutDesc::GetInstance()->GetVertexDesc(graphic_pipeline_desc.vertex_shader_file + "::" + graphic_pipeline_desc.vertex_shader_func);
		desc_out.InputLayout.pInputElementDescs = vertex_desc->inputElementDescs;
		desc_out.InputLayout.NumElements = static_cast<UINT>(vertex_desc->input_element_num);
		//读取当前pipeline的资源绑定格式
		check_error = ParseDiscriptorDistribution(graphic_pipeline_desc.descriptor_type);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//创建资源
		HRESULT hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateGraphicsPipelineState(&desc_out, IID_PPV_ARGS(&pso_data));
		if (FAILED(hr))
		{
			LResult error_message;
			LunarDebugLogError(hr, "Create PSO error name " + pso_name.str(), error_message);

			return error_message;
		}
	}
	else if (pipline_type == PSOType::PSO_TYPE_COMPUTE)
	{
		//读取管线的信息
		PipelineStateDescCompute compute_pipeline_desc;
		check_error = LSrtructReflectControl::GetInstance()->DeserializeFromJsonFile(compute_pipeline_desc, file_name);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//根据读取的信息建立管线数据
		check_error = PancyRootSignatureControl::GetInstance()->GetRootSignature(compute_pipeline_desc.root_signature_file, root_signature_ID);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		ID3D12RootSignature* root_signature_use;
		PancyRootSignatureControl::GetInstance()->GetResource(root_signature_ID, &root_signature_use);

		D3D12_COMPUTE_PIPELINE_STATE_DESC  desc_out = {};
		desc_out.pRootSignature = root_signature_use;
		//计算着色器pipeline
		if (compute_pipeline_desc.compute_shader_file != "0" && compute_pipeline_desc.compute_shader_func != "0")
		{
			//获取shader数据
			ComPtr<ID3DBlob> shader_data;
			check_error = PancyShaderControl::GetInstance()->GetShaderData(compute_pipeline_desc.compute_shader_file, compute_pipeline_desc.compute_shader_func, "cs_5_1", &shader_data);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			//获取shader reflect并存储cbuffer的信息
			ComPtr<ID3D12ShaderReflection> now_shader_reflect;
			check_error = PancyShaderControl::GetInstance()->GetShaderReflection(compute_pipeline_desc.compute_shader_file, compute_pipeline_desc.compute_shader_func, "cs_5_1", &now_shader_reflect);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			check_error = BuildCbufferByShaderReflect(now_shader_reflect);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			//填充shader信息
			desc_out.CS = CD3DX12_SHADER_BYTECODE(shader_data.Get());
			//读取当前pipeline的资源绑定格式
			check_error = ParseDiscriptorDistribution(compute_pipeline_desc.descriptor_type);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			//创建资源
			HRESULT hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateComputePipelineState(&desc_out, IID_PPV_ARGS(&pso_data));
			if (FAILED(hr))
			{
				LResult error_message;
				LunarDebugLogError(hr, "Create PSO error name " + pso_name.str(), error_message);
				return error_message;
			}
		}
	}
	return g_Succeed;
}
const std::vector<PancyDescriptorPSODescription>& PancyPiplineStateObjectGraph::GetDescriptor(const PancyShaderDescriptorType& descriptor_type)
{
	switch (descriptor_type)
	{
	case PancyShaderDescriptorType::CbufferGlobel:
		return globel_cbuffer;
		break;
	case PancyShaderDescriptorType::CbufferPrivate:
		return private_cbuffer;
		break;
	case PancyShaderDescriptorType::SRVGlobel:
		return globel_shader_res;
		break;
	case PancyShaderDescriptorType::SRVPrivate:
		return private_shader_res;
		break;
	case PancyShaderDescriptorType::SRVBindless:
		return bindless_shader_res;
		break;
	default:
		break;
	}
	LResult error_message;
	LunarDebugLogError(E_FAIL, "unrecognized PSO descriptor type", error_message);

	return globel_cbuffer;
}
LResult PancyPiplineStateObjectGraph::GetInputDesc(ComPtr<ID3D12ShaderReflection> t_ShaderReflection, std::vector<D3D12_INPUT_ELEMENT_DESC>& t_InputElementDescVec)
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
PancyPiplineStateObjectGraph::~PancyPiplineStateObjectGraph()
{
	for (auto cbuffer_alloctor_value = Cbuffer_map.begin(); cbuffer_alloctor_value != Cbuffer_map.end(); ++cbuffer_alloctor_value)
	{
		delete cbuffer_alloctor_value->second;
	}
	Cbuffer_map.clear();
}
//effect
PancyEffectGraphic::PancyEffectGraphic()
{
	PSO_id_self_add = 0;
	AddPSOGlobelVariable();
	AddJsonReflect();
}
void PancyEffectGraphic::AddPSOGlobelVariable()
{
	//PSO格式
	InitNewEnumValue(PSO_TYPE_GRAPHIC);
	InitNewEnumValue(PSO_TYPE_COMPUTE);
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
void PancyEffectGraphic::AddJsonReflect()
{
	InitNewStructToReflection(RootSignatureParameterDesc);
	InitNewStructToReflection(D3D12_STATIC_SAMPLER_DESC);
	InitNewStructToReflection(RootSignatureDesc);
	InitNewStructToReflection(DescriptorTypeDesc);
	InitNewStructToReflection(PipelineStateDescCompute);
	InitNewStructToReflection(D3D12_RENDER_TARGET_BLEND_DESC);
	InitNewStructToReflection(PipelineStateDescGraphic);
}
LResult PancyEffectGraphic::BuildPso(const std::string& pso_config_file)
{
	LResult check_error;
	//加载一个新的PSO结构
	PancyPiplineStateObjectGraph* new_pancy = new PancyPiplineStateObjectGraph(pso_config_file);
	check_error = new_pancy->Create();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//根据ID池是否为空来决定新开辟的资源ID号
	LunarObjectID now_next_id;
	if (empty_PSO_id.size() > 0)
	{
		now_next_id = empty_PSO_id.front();
		empty_PSO_id.pop();
	}
	else
	{
		now_next_id = PSO_id_self_add;
		PSO_id_self_add += 1;
	}
	//将资源插入到资源表
	PSO_name.insert(std::pair<std::string, LunarObjectID>(pso_config_file, now_next_id));
	PSO_name_reflect.insert(std::pair<LunarObjectID, std::string>(now_next_id, pso_config_file));
	PSO_array.insert(std::pair<LunarObjectID, PancyPiplineStateObjectGraph*>(now_next_id, new_pancy));
	return g_Succeed;
}
LResult PancyEffectGraphic::GetPSOName(const LunarObjectID& PSO_id, std::string& pso_name_out)
{
	auto PSO_array_find = PSO_name_reflect.find(PSO_id);
	if (PSO_array_find == PSO_name_reflect.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id), error_message);

		return error_message;
	}
	pso_name_out = PSO_array_find->second;
	return g_Succeed;
}
LResult PancyEffectGraphic::GetPSOResource(const LunarObjectID& PSO_id, ID3D12PipelineState** PSO_res)
{
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id), error_message);

		return error_message;
	}
	PSO_array_find->second->GetResource(PSO_res);
	return g_Succeed;
}
LResult PancyEffectGraphic::GetRootSignatureResource(const LunarObjectID& PSO_id, ID3D12RootSignature** RootSig_res)
{
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id), error_message);

		return error_message;
	}
	PancyRootSignatureControl::GetInstance()->GetResource(PSO_array_find->second->GetRootSignature(), RootSig_res);
	return g_Succeed;
}
/*
EngineFailReason PancyEffectGraphic::GetPSODescriptorName(const LunarObjectID &PSO_id, std::string &descriptor_heap_name)
{
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		EngineFailReason error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id),error_message);

		return error_message;
	}
	PSO_array_find->second->GetDescriptorHeapUse(descriptor_heap_name);
	return succeed;
}
EngineFailReason PancyEffectGraphic::GetDescriptorDistribute(const LunarObjectID &PSO_id, std::vector<LunarObjectID> &descriptor_distribute)
{
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		EngineFailReason error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id),error_message);

		return error_message;
	}
	PSO_array_find->second->GetDescriptorDistribute(descriptor_distribute);
	return succeed;
}
*/
LResult PancyEffectGraphic::GetPSO(const std::string& name_in, LunarObjectID& PSO_id)
{
	LResult check_error;
	auto PSO_array_find = PSO_name.find(name_in);
	if (PSO_array_find == PSO_name.end())
	{
		//未找到PSO，尝试加载
		check_error = BuildPso(name_in);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		PSO_array_find = PSO_name.find(name_in);
	}
	PSO_id = PSO_array_find->second;
	return g_Succeed;
}
/*
EngineFailReason PancyEffectGraphic::CheckCbuffer(const LunarObjectID &PSO_id, const std::string &name_in)
{
	EngineFailReason check_error;
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		EngineFailReason error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id),error_message);

		return error_message;
	}
	check_error = PSO_array_find->second->CheckCbuffer(name_in);
	if (!check_error.if_succeed)
	{
		return check_error;
	}
	return succeed;
}
EngineFailReason PancyEffectGraphic::GetCbuffer(const LunarObjectID &PSO_id, const std::string &cbuffer_name, const Json::Value *& CbufferData)
{
	EngineFailReason check_error;
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		EngineFailReason error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id),error_message);

		return error_message;
	}
	check_error = PSO_array_find->second->GetCbuffer(cbuffer_name, CbufferData);
	if (!check_error.if_succeed)
	{
		return check_error;
	}
	return succeed;
}
*/
LResult PancyEffectGraphic::BuildCbufferByName(
	const LunarObjectID& PSO_id,
	const std::string& cbuffer_name,
	PancyConstantBuffer& cbuffer_data_out
)
{
	LResult check_error;
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id), error_message);

		return error_message;
	}
	check_error = PSO_array_find->second->BuildCbufferByName(cbuffer_name, cbuffer_data_out);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancyEffectGraphic::ReleaseCbufferByID(
	const LunarObjectID& PSO_id,
	const std::string& cbuffer_name,
	const LunarObjectID& buffer_resource_id,
	const LunarObjectID& buffer_offset_id
)
{
	LResult check_error;
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id), error_message);

		return error_message;
	}
	check_error = PSO_array_find->second->ReleaseCbufferByID(cbuffer_name, buffer_resource_id, buffer_offset_id);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}

LResult PancyEffectGraphic::GetDescriptorDesc(
	const LunarObjectID& PSO_id,
	const PancyShaderDescriptorType& descriptor_type,
	const std::vector<PancyDescriptorPSODescription>*& descriptor_param_data
)
{
	auto PSO_array_find = PSO_array.find(PSO_id);
	if (PSO_array_find == PSO_array.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find the PSO ID:" + std::to_string(PSO_id), error_message);

		return error_message;
	}
	const std::vector<PancyDescriptorPSODescription>& pre_data = PSO_array_find->second->GetDescriptor(descriptor_type);
	descriptor_param_data = &pre_data;
	return g_Succeed;
}
PancyEffectGraphic::~PancyEffectGraphic()
{
	for (auto data = PSO_array.begin(); data != PSO_array.end(); ++data)
	{
		delete data->second;
	}
	PSO_array.clear();
}


ConstantBufferAlloctor::ConstantBufferAlloctor(
	const LunarResourceSize& cbuffer_size_in,
	const std::string& cbuffer_name_in,
	const std::string& cbuffer_effect_name_in,
	const PancyCommonBufferDesc& buffer_resource_desc_value_in,
	Json::Value& cbuffer_desc_value_in
)
{
	cbuffer_size = cbuffer_size_in;
	cbuffer_name = cbuffer_name_in;
	cbuffer_effect_name = cbuffer_effect_name_in;
	buffer_resource_desc_value = buffer_resource_desc_value_in;
	cbuffer_desc_value = cbuffer_desc_value_in;
}
LResult ConstantBufferAlloctor::BuildNewCbuffer(PancyConstantBuffer& cbuffer_data)
{
	CbufferPackList* now_used_buffer_resource = NULL;
	//先从现有的buffer中寻找有没有空闲的存储块
	for (auto buffer_resource_data = all_cbuffer_list.begin(); buffer_resource_data != all_cbuffer_list.end(); ++buffer_resource_data)
	{
		if (buffer_resource_data->second->now_empty_offset.size() > 0)
		{
			now_used_buffer_resource = buffer_resource_data->second;
			break;
		}
	}
	//如果现有的buffer存储块都已经被使用了，则新开一个空的buffer
	if (now_used_buffer_resource == NULL)
	{
		VirtualResourcePointer new_buffer_resource;
		auto check_error = BuildBufferResource(
			cbuffer_effect_name + "::" + cbuffer_name,
			buffer_resource_desc_value,
			new_buffer_resource,
			true
		);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		now_used_buffer_resource = new CbufferPackList(new_buffer_resource, cbuffer_size);
		all_cbuffer_list.insert(std::pair<LunarObjectID, CbufferPackList*>(new_buffer_resource.GetResourceId(), now_used_buffer_resource));
	}
	//从新的存储块的指定偏移点创建一个新的cbuffer
	auto now_alloc_data = *now_used_buffer_resource->now_empty_offset.begin();
	auto check_error = cbuffer_data.Create(
		cbuffer_name,
		cbuffer_effect_name,
		now_used_buffer_resource->buffer_pointer,
		now_alloc_data,
		cbuffer_size,
		cbuffer_desc_value
	);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//标记指定的存储块已经被使用
	now_used_buffer_resource->now_empty_offset.erase(now_alloc_data);
	now_used_buffer_resource->now_use_offset.insert(now_alloc_data);
	return g_Succeed;
}
LResult ConstantBufferAlloctor::ReleaseCbuffer(const LunarObjectID& buffer_resource_id, const LunarObjectID& buffer_offset_id)
{
	//先检查给定的资源是否是已经开辟的资源
	auto buffer_resource_pointer = all_cbuffer_list.find(buffer_resource_id);
	if (buffer_resource_pointer == all_cbuffer_list.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find buffer resource: " + std::to_string(buffer_resource_id), error_message);

		return error_message;
	}
	if (buffer_resource_pointer->second->now_use_offset.find(buffer_offset_id) == buffer_resource_pointer->second->now_use_offset.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find cbuffer offset: " + std::to_string(buffer_offset_id), error_message);

		return error_message;
	}
	//将资源的偏移量释放
	buffer_resource_pointer->second->now_use_offset.erase(buffer_offset_id);
	buffer_resource_pointer->second->now_empty_offset.insert(buffer_offset_id);
	//如果释放完毕后资源没有任何引用则删除掉该资源
	if (buffer_resource_pointer->second->now_use_offset.size() == 0)
	{
		delete buffer_resource_pointer->second;
		all_cbuffer_list.erase(buffer_resource_pointer);
	}
	return g_Succeed;
}
//常量缓冲区
LResult PancyConstantBuffer::SetMatrix(const std::string& variable, const DirectX::XMFLOAT4X4& mat_data, const LunarResourceSize& offset)
{
	auto start_pos = member_variable.find(variable);
	if (start_pos == member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	//传入Cbuffer中的矩阵需要做转置操作
	DirectX::XMFLOAT4X4 transpose_mat;
	DirectX::XMStoreFloat4x4(&transpose_mat, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&mat_data)));
	memcpy(map_pointer_out + start_pos->second.start_offset + offset * sizeof(DirectX::XMFLOAT4X4), &transpose_mat, sizeof(transpose_mat));
	return g_Succeed;
}
LResult PancyConstantBuffer::SetFloat4(const std::string& variable, const DirectX::XMFLOAT4& vector_data, const LunarResourceSize& offset)
{
	auto start_pos = member_variable.find(variable);
	if (start_pos == member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	memcpy(map_pointer_out + start_pos->second.start_offset + offset * sizeof(DirectX::XMFLOAT4), &vector_data, sizeof(vector_data));
	return g_Succeed;
}
LResult PancyConstantBuffer::SetUint4(const std::string& variable, const DirectX::XMUINT4& vector_data, const LunarResourceSize& offset)
{
	auto start_pos = member_variable.find(variable);
	if (start_pos == member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	memcpy(map_pointer_out + start_pos->second.start_offset + offset * sizeof(DirectX::XMUINT4), &vector_data, sizeof(vector_data));
	return g_Succeed;
}
LResult PancyConstantBuffer::SetStruct(const std::string& variable, const void* struct_data, const LunarResourceSize& data_size, const LunarResourceSize& offset)
{
	auto start_pos = member_variable.find(variable);
	if (start_pos == member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	memcpy(map_pointer_out + start_pos->second.start_offset + offset * data_size, struct_data, data_size);
	return g_Succeed;
}
LResult PancyConstantBuffer::ErrorVariableNotFind(const std::string& variable_name)
{
	LResult error_message;
	LunarDebugLogError(E_FAIL, "Could not find shader variable:" + variable_name + " in Cbuffer: " + cbuffer_name + " PSO:" + cbuffer_effect_name, error_message);

	return error_message;
}
PancyConstantBuffer::PancyConstantBuffer()
{
}
LResult PancyConstantBuffer::Create(
	const std::string& cbuffer_name_in,
	const std::string& cbuffer_effect_name_in,
	const VirtualResourcePointer& buffer_id_in,
	const LunarResourceSize& buffer_offset_id_in,
	const LunarResourceSize& cbuffer_size_in,
	const Json::Value& root_value
)
{
	//填充基本信息
	cbuffer_name = cbuffer_name_in;
	cbuffer_effect_name = cbuffer_effect_name_in;
	buffer_ID = buffer_id_in;
	buffer_offset_id = static_cast<LunarObjectID>(buffer_offset_id_in);
	const PancyBasicBuffer* pointer = dynamic_cast<const PancyBasicBuffer*>(buffer_ID.GetResourceData());
	cbuffer_size = cbuffer_size_in;
	map_pointer_out = pointer->GetBufferCPUPointer() + buffer_offset_id * cbuffer_size;
	//从json文件中读取cbuffer的布局反射
	LResult check_error;
	std::string cbuffer_final_name = cbuffer_effect_name + "_" + cbuffer_name;
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = GetCbufferDesc(cbuffer_final_name, root_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	if_loaded = true;
	return g_Succeed;
}
LResult PancyConstantBuffer::GetCbufferDesc(const std::string& file_name, const Json::Value& root_value)
{
	LResult check_error;
	pancy_json_value now_value;
	Json::Value value_cbuffer_desc = root_value.get("CbufferDesc", Json::Value::null);
	if (value_cbuffer_desc == Json::Value::null)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find value: CbufferDesc of variable: " + file_name, error_message);

		return error_message;
	}
	//读取缓冲区大小
	check_error = PancyJsonTool::GetInstance()->GetJsonData(file_name, value_cbuffer_desc, "BufferSize", pancy_json_data_type::json_data_int, now_value);
	cbuffer_size = static_cast<LunarResourceSize>(now_value.int_value);
	//读取常量缓冲区的所有变量
	Json::Value value_cbuffer_member = value_cbuffer_desc.get("VariableMember", Json::Value::null);
	if (value_cbuffer_member == Json::Value::null)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find value: VariableMember of variable: " + file_name, error_message);

		return error_message;
	}
	for (Json::ArrayIndex i = 0; i < static_cast<Json::ArrayIndex>(value_cbuffer_member.size()); ++i)
	{
		CbufferVariable new_variable_data;
		std::string variable_name;
		Json::Value value_cbuffer_variable;
		value_cbuffer_variable = value_cbuffer_member.get(i, Json::nullValue);
		//读取变量名称
		check_error = PancyJsonTool::GetInstance()->GetJsonData(file_name, value_cbuffer_variable, "Name", pancy_json_data_type::json_data_string, now_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		variable_name = now_value.string_value;
		//读取变量大小
		check_error = PancyJsonTool::GetInstance()->GetJsonData(file_name, value_cbuffer_variable, "Size", pancy_json_data_type::json_data_int, now_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		new_variable_data.variable_size = now_value.int_value;
		//读取变量偏移
		check_error = PancyJsonTool::GetInstance()->GetJsonData(file_name, value_cbuffer_variable, "StartOffset", pancy_json_data_type::json_data_int, now_value);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		new_variable_data.start_offset = now_value.int_value;
		//记录当前的新变量
		member_variable.insert(std::pair<std::string, CbufferVariable>(variable_name, new_variable_data));
	}
	return g_Succeed;
}
PancyConstantBuffer::~PancyConstantBuffer()
{
	if (if_loaded)
	{
		LunarObjectID effect_pso_id;
		auto check_error = PancyEffectGraphic::GetInstance()->GetPSO(cbuffer_effect_name, effect_pso_id);
		if (!check_error.m_IsOK)
		{
			return;
		}
		check_error = PancyEffectGraphic::GetInstance()->ReleaseCbufferByID(effect_pso_id, cbuffer_name, buffer_ID.GetResourceId(), buffer_offset_id);
		if (!check_error.m_IsOK)
		{
			return;
		}
		member_variable.clear();
	}
}
