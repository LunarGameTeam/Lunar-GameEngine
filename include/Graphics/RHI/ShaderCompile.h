#pragma once
#include "Graphics/RenderConfig.h"

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <Vulkan/vulkan.h>
#include <wrl/client.h>
#include <dxc/dxcapi.h>

#include "Core/Asset/AssetModule.h"
#include "Graphics/RHI/RHIPch.h"

namespace luna::graphics
{

bool DxcCompile(
	RHIShaderType stage,
	const LString& name,
	const LString& pShader,
	std::vector<uint32_t>& spirv,
	Microsoft::WRL::ComPtr<IDxcBlob>& code,
	const LArray<RhiShaderMacro>& mShaderMacros
);

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