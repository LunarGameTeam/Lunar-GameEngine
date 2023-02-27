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
	DxcCompile(resource_desc.mType, resource_desc.mName, resource_desc.mContent, dst, code);
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
		RHICBufferDesc cb;
		const auto& it = shadeReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC dxShaderDesc;
		it->GetDesc(&dxShaderDesc);
		cb.mName = dxShaderDesc.Name;
		size_t hash = cb.mName.Hash();
		if (HasBindPoint(LString(dxShaderDesc.Name).Hash()))
			continue;
		cb.mSize = SizeAligned2Pow(dxShaderDesc.Size, 256);

		for (uint32_t idx = 0; idx < dxShaderDesc.Variables; ++idx)
		{
			CBufferVar cbVar;
			auto uniforms = it->GetVariableByIndex(idx);
			D3D12_SHADER_VARIABLE_DESC varDesc;
			uniforms->GetDesc(&varDesc);
			size_t varHash = LString(varDesc.Name).Hash();
			cbVar.mOffset = varDesc.StartOffset;
			cbVar.mSize = varDesc.Size;
			cbVar.mName = varDesc.Name;
			cb.mVars[varHash]  =  cbVar;
		}
		mUniformBuffers[hash] =  cb;
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
