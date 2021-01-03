#include "LDirectX12Shader.h"
using namespace luna;
using Microsoft::WRL::ComPtr;
void LDx12GraphicResourceShaderBlob::CheckIfLoadingStateChanged(LLoadState& m_object_load_state)
{
	m_object_load_state = LLoadState::LOAD_STATE_FINISHED;
};
luna::LResult LDx12GraphicResourceShaderBlob::InitResorceByDesc(const LunarShaderMessage& resource_desc)
{
	if (shader_type_string.size() == 0)
	{
		//注册shader类型名
		shader_type_string.insert(std::pair<LunarGraphicShaderType, LString>(LunarGraphicShaderType::SHADER_VERTEX, "vs_5_1"));
		shader_type_string.insert(std::pair<LunarGraphicShaderType, LString>(LunarGraphicShaderType::SHADER_PIXEL, "ps_5_1"));
		shader_type_string.insert(std::pair<LunarGraphicShaderType, LString>(LunarGraphicShaderType::SHADER_GEOMETRY, "gs_5_1"));
		shader_type_string.insert(std::pair<LunarGraphicShaderType, LString>(LunarGraphicShaderType::SHADER_COMPUTE, "cs_5_1"));
		shader_type_string.insert(std::pair<LunarGraphicShaderType, LString>(LunarGraphicShaderType::SHADER_HULL, "hs_5_1"));
		shader_type_string.insert(std::pair<LunarGraphicShaderType, LString>(LunarGraphicShaderType::SHADER_DOMIN, "ds_5_1"));
	}

#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#else
	UINT compileFlags = 0 | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#endif
	//编译shader文件
	ComPtr<ID3DBlob> error_message = NULL;
	HRESULT hr = D3DCompileFromFile(
		StringToWstring(resource_desc.shader_path.c_str()).c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		resource_desc.shader_entry_point_name.c_str(),
		shader_type_string[resource_desc.shader_type].c_str(),
		compileFlags,
		0,
		&shader_memory_pointer,
		&error_message
	);
	if (FAILED(hr))
	{
		char data[4096];
		if (error_message != NULL)
		{
			memcpy(data, error_message->GetBufferPointer(), error_message->GetBufferSize());
		}
		LResult error_message;
		LunarDebugLogError(hr, "Compile shader : " + resource_desc.shader_path + ":: " + resource_desc.shader_entry_point_name + " error:" + data, error_message);
		return error_message;
	}
	//获取shader反射
	hr = D3DReflect(shader_memory_pointer->GetBufferPointer(), shader_memory_pointer->GetBufferSize(), IID_ID3D12ShaderReflection, &shader_reflection);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "get shader reflect message : " + resource_desc.shader_path + ":: " + resource_desc.shader_entry_point_name + " error", error_message);
		return error_message;
	}
	return luna::g_Succeed;
}