#include "DX12Shader.h"
#include "Graphics/RHI/ShaderCompile.h"
using namespace luna;
using Microsoft::WRL::ComPtr;


namespace luna::graphics
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
	DxcCompile(resource_desc.mType, resource_desc.mName, resource_desc.mContent, dst, code, resource_desc.mShaderMacros);
	D3DCreateBlob(dst.size() * 4, &mShaderBlob);
	
	memcpy(mShaderBlob->GetBufferPointer(), dst.data(), mShaderBlob->GetBufferSize());	
	//获取shader反射
	hr = sReflection->Load(code.Get());
	assert(SUCCEEDED(hr));
	uint32_t partIndex;
	sReflection->FindFirstPartKind(DXC_PART_DXIL,  &partIndex);
	hr = sReflection->GetPartReflection(partIndex, IID_PPV_ARGS(&mShaderReflection));
	assert(SUCCEEDED(hr));

	D3D12_SHADER_DESC shaderDesc;
	mShaderReflection->GetDesc(&shaderDesc);
	for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		RHICBufferDesc cb;
		const auto& it = mShaderReflection->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC dxShaderDesc;
		it->GetDesc(&dxShaderDesc);
		cb.mName = dxShaderDesc.Name;
		size_t hash = cb.mName.Hash();
		if (dxShaderDesc.Type != D3D_CT_CBUFFER || HasBindPoint(LString(dxShaderDesc.Name).Hash()))
			continue;
		cb.mSize = SizeAligned2Pow(dxShaderDesc.Size, 256);

		for (uint32_t idx = 0; idx < dxShaderDesc.Variables; ++idx)
		{
			CBufferVar cbVar;
			ID3D12ShaderReflectionVariable* uniforms = it->GetVariableByIndex(idx);
			D3D12_SHADER_VARIABLE_DESC varDesc;
			uniforms->GetDesc(&varDesc);
			ID3D12ShaderReflectionType* variableType = uniforms->GetType();
			D3D12_SHADER_TYPE_DESC variableTypeDesc;
			variableType->GetDesc(&variableTypeDesc);
			size_t varHash = LString(varDesc.Name).Hash();
			cbVar.mOffset = varDesc.StartOffset;
			cbVar.mSize = varDesc.Size;
			cbVar.mName = varDesc.Name;
			if (variableTypeDesc.Elements > 0)
			{
				cbVar.mIsArray = true;
				cbVar.mElementSize = variableTypeDesc.Rows * variableTypeDesc.Columns * sizeof(float);
			}
			if (variableTypeDesc.Class == D3D_SHADER_VARIABLE_CLASS::D3D10_SVC_STRUCT)
			{
				cbVar.mIsStruct = true;
				for (int32_t memberIndex = 0; memberIndex < variableTypeDesc.Members; ++memberIndex)
				{
					ID3D12ShaderReflectionType* variableMemberType = variableType->GetMemberTypeByIndex(memberIndex);
					D3D12_SHADER_TYPE_DESC variableMemberTypeDesc;
					variableMemberType->GetDesc(&variableMemberTypeDesc);
					size_t varMemberHash = LString(variableMemberTypeDesc.Name).Hash();
					CBufferVar cbMemberVar;
					cbVar.mOffset = variableMemberTypeDesc.Offset;
					cbVar.mSize = variableMemberTypeDesc.Rows * variableMemberTypeDesc.Columns * sizeof(float);
					cbVar.mName = variableMemberTypeDesc.Name;
					cbVar.mStructVars.insert({ varMemberHash,cbVar});
				}
				
			}

			cb.mVars[varHash]  =  cbVar;
		}
		mUniformBuffers[hash] =  cb;
	}


	//记录资源绑定信息
	for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D12_SHADER_INPUT_BIND_DESC shaderInputDesc;
		mShaderReflection->GetResourceBindingDesc(i, &shaderInputDesc);
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
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
			bindKey.mViewType = RHIViewType::kStructuredBuffer;
			break;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
			bindKey.mViewType = RHIViewType::kRWStructuredBuffer;
			break;
		default:
			assert(false);
			break;
		}
		if (bindKey.mViewType == RHIViewType::kConstantBuffer && bindKey.mSpace == 4)
		{
			LString curName = shaderInputDesc.Name;
			auto bufferDesc = mUniformBuffers[curName.Hash()];
			RHIPushConstantValue bindValue;
			bindValue.mName = shaderInputDesc.Name;
			bindValue.mOffset = 0;
			bindValue.mSize = bufferDesc.mSize;
			bindValue.mRegisterID = shaderInputDesc.BindPoint;
			bindValue.mRegisterSpace = shaderInputDesc.Space;
			for (auto eachValue : bufferDesc.mVars)
			{
				RHIPushConstantMember mMember;
				mMember.mName = eachValue.second.mName;
				mMember.mOffset = eachValue.second.mOffset;
				mMember.mSize = eachValue.second.mSize;
				bindValue.mValueMember.insert({ eachValue.second.mName,mMember });
			}
			mBindConstants[bindKey.mName.Hash()] = bindValue;
			mUniformBuffers.erase(curName.Hash());
		}
		else
		{
			mBindPoints[bindKey.mName.Hash()] = bindKey;
		}
	}
	return true;
}
}
