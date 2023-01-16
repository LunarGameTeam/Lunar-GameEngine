#include "DX12Shader.h"
#include "Graphics/RHI/ShaderCompile.h"
using namespace luna;
using Microsoft::WRL::ComPtr;


namespace luna::render
{

Microsoft::WRL::ComPtr<IDxcContainerReflection> sReflection;

RENDER_API LUnorderedMap<RHIShaderType, LString> DX12ShaderBlob::shader_type_string;

DX12ShaderBlob::DX12ShaderBlob(const RHIShaderDesc& resource_desc) : RHIShaderBlob(
	resource_desc)
{
	InitShader(resource_desc);
}

bool DX12ShaderBlob::InitShader(const RHIShaderDesc& resource_desc)
{
	if (!sReflection)
	{
		DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(sReflection.GetAddressOf()));
	}
	if (shader_type_string.size() == 0)
	{
		//注册shader类型名
		shader_type_string.insert(
			std::pair<RHIShaderType, LString>(RHIShaderType::Vertex, "vs_5_1"));
		shader_type_string.insert(
			std::pair<RHIShaderType, LString>(RHIShaderType::Pixel, "ps_5_1"));
		shader_type_string.insert(
			std::pair<RHIShaderType, LString>(RHIShaderType::Geomtry, "gs_5_1"));
		shader_type_string.insert(
			std::pair<RHIShaderType, LString>(RHIShaderType::Compute, "cs_5_1"));
		shader_type_string.insert(
			std::pair<RHIShaderType, LString>(RHIShaderType::Hull, "hs_5_1"));
		shader_type_string.insert(
			std::pair<RHIShaderType, LString>(RHIShaderType::Domin, "ds_5_1"));
	}
#if defined(_DEBUG)
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION |
		D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#else
			UINT compileFlags = 0 | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#endif
	ComPtr<ID3DBlob> error_message = NULL;

	HRESULT hr;

	std::vector<uint32_t> dst;
	ComPtr<IDxcBlob> code;
	DxcCompile(resource_desc.mType, resource_desc.mContent, dst, code);	
	D3DCreateBlob(dst.size() * 4, &mShaderBlob);
	
	memcpy(mShaderBlob->GetBufferPointer(), dst.data(), mShaderBlob->GetBufferSize());	
	//获取shader反射
	hr = sReflection->Load(code.Get());
	assert(SUCCEEDED(hr));
	uint32_t partIndex;
	sReflection->FindFirstPartKind(DXC_PART_DXIL,  &partIndex);
	hr = sReflection->GetPartReflection(partIndex, IID_PPV_ARGS(&mShaderReflection));
	assert(SUCCEEDED(hr));

	auto shadeReflection = GetDX12ShaderReflection();
	D3D12_SHADER_DESC shaderDesc;
	shadeReflection->GetDesc(&shaderDesc);
	for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		RHIConstantBufferDesc uniformDesc;
		auto now_constant_buffer = shadeReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
		now_constant_buffer->GetDesc(&shaderBufferDesc);
		
		if (HasBindPoint(LString(shaderBufferDesc.Name).Hash()))
			continue;

		uniformDesc.mBufferSize = SizeAligned2Pow(shaderBufferDesc.Size, 256);
		for (uint32_t uniform_id = 0; uniform_id < shaderBufferDesc.Variables; ++uniform_id)
		{
			ConstantBufferVar uniformVar;
			auto uniforms = now_constant_buffer->GetVariableByIndex(uniform_id);
			D3D12_SHADER_VARIABLE_DESC uniform_desc;
			uniforms->GetDesc(&uniform_desc);
			uniformVar.mOffset = uniform_desc.StartOffset;
			uniformVar.mSize = uniform_desc.Size;
			uniformDesc.mVars.insert(std::pair<LString, ConstantBufferVar>(uniform_desc.Name, uniformVar));
		}
		mUniformBuffers.insert(
			std::pair<LString, RHIConstantBufferDesc>(shaderBufferDesc.Name, uniformDesc));
	}


	//记录资源绑定信息
	for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D12_SHADER_INPUT_BIND_DESC shaderInputDesc;
		shadeReflection->GetResourceBindingDesc(i, &shaderInputDesc);
		RHIBindPoint bindKey;
		bindKey.mName = shaderInputDesc.Name;
		bindKey.mSlot = shaderInputDesc.BindPoint;
		bindKey.mSpace = shaderInputDesc.Space;
		switch (shaderInputDesc.Type)
		{
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			bindKey.mViewType = RHIViewType::kConstantBuffer;
			break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			bindKey.mViewType = RHIViewType::kTexture;
			break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
			bindKey.mViewType = RHIViewType::kSampler;
			break;
		default:
			assert(false);
			break;
		}
		mBindPoints[bindKey.mName.Hash()] = bindKey;
	}

	return true;
}
}
