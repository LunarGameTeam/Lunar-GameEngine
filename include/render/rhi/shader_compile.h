#pragma once
#include "render/render_config.h"

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <vulkan/vulkan.h>
#include <wrl/client.h>
#include <dxc/dxcapi.h>

#include "core/asset/asset_module.h"
#include "render/rhi/rhi_pch.h"

namespace luna::render
{

bool DxcCompile(RHIShaderType stage, const LString& pShader, std::vector<uint32_t>& spirv,
	Microsoft::WRL::ComPtr<IDxcBlob>& code);

class LunaIncludeHandler : public IDxcIncludeHandler
{

public:
	std::unordered_set<LString> IncludedFiles;

	virtual HRESULT STDMETHODCALLTYPE LoadSource(_In_z_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource);


	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override;


	ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
	ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

};

}