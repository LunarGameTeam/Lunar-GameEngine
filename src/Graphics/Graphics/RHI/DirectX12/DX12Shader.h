#pragma once
#include "Graphics/RenderConfig.h"
#include "DX12Pch.h"
#include "Graphics/RHI/RHIShader.h"


namespace luna::render
{
class RENDER_API DX12ShaderBlob : public RHIShaderBlob
{
	Microsoft::WRL::ComPtr<ID3DBlob> mShaderBlob;
	Microsoft::WRL::ComPtr<ID3D12ShaderReflection> mShaderReflection;
	static LUnorderedMap<RHIShaderType, LString> shader_type_string;
public:
	DX12ShaderBlob(const RHIShaderDesc& resource_desc);

	inline ID3DBlob* GetDX12Blob() const
	{
		return mShaderBlob.Get();
	}

	inline ID3D12ShaderReflection* GetDX12ShaderReflection()
	{
		return mShaderReflection.Get();
	}

private:
	bool InitShader(const RHIShaderDesc& resource_desc);
};
}
