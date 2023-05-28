#include "Graphics/RHI/ShaderCompile.h"


#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/RenderModule.h"

using namespace Microsoft::WRL;

namespace luna::render
{

static Microsoft::WRL::ComPtr<IDxcLibrary> sDxcLib;
static Microsoft::WRL::ComPtr<IDxcCompiler> sDxcCompiler;
static Microsoft::WRL::ComPtr<IDxcIncludeHandler> sDefaultIncludeHandler;

std::wstring CompileStringToLPCWSTR(std::string orig)
{
	std::wstring newWstring;
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	newWstring.resize(orig.length() - 1);
	mbstowcs_s(&convertedChars, newWstring.data(), origsize, orig.c_str(), _TRUNCATE);
	return newWstring;
}

bool DxcCompile(
	RHIShaderType stage,
	const LString& name,
	const LString& pShader,
	std::vector<uint32_t>& spirv,
	ComPtr<IDxcBlob>& code,
	const LArray<RhiShaderMacro> &mShaderMacros
)
{
	HRESULT hres = S_OK;
	if (!sDxcLib)
		hres = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(sDxcLib.GetAddressOf()));
	if (!sDxcCompiler)
		hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&sDxcCompiler));
	LunaIncludeHandler includeHandler;
	if (FAILED(hres)) {
		throw std::runtime_error("Could not init DXC Library");
	}

	if (FAILED(hres)) {
		throw std::runtime_error("Could not init DXC Compiler");
	}
	ComPtr<IDxcBlobEncoding> sourceBlob;
	uint32_t codePage = CP_UTF8;
	sDxcLib->CreateBlobWithEncodingOnHeapCopy(pShader.c_str(), (uint32_t)pShader.Length(), codePage, &sourceBlob);
	ComPtr<IDxcOperationResult> result;
	
	LPCWSTR shaderStageName;
	LPCWSTR shaderEntry;
	LPCWSTR shaderTarget;
	if (sRenderModule->mRenderContext->mDeviceType == RenderDeviceType::Vulkan)
		shaderTarget = L"-spirv";
	else
		shaderTarget = L"";

	if (stage == RHIShaderType::Vertex)
	{
		shaderStageName = L"vs_6_0";
		shaderEntry = L"VSMain";
	}
	else
	{
		shaderStageName = L"ps_6_0";
		shaderEntry = L"PSMain";
	}

	std::vector<LPCWSTR> arguments = {
		// (Optional) name of the shader file to be displayed e.g. in an error message
		L"",
		shaderTarget,
#ifdef _DEBUG
		DXC_ARG_DEBUG,
		L"-Od",
		L"-fspv-debug=vulkan-with-source",
#endif
	};

	DxcDefine* dxcMacroDefinePointer = nullptr;
	LArray<DxcDefine> dxcMacroDefine;
	LArray<std::wstring> nameTable;
	for (auto& eachMacro : mShaderMacros)
	{
		DxcDefine dxcMacroMember;
		nameTable.push_back(CompileStringToLPCWSTR(eachMacro.mMacroName.c_str()));
		dxcMacroMember.Name = nameTable.back().data();
		nameTable.push_back(CompileStringToLPCWSTR(eachMacro.mMacroValue.c_str()));
		dxcMacroMember.Value = nameTable.back().data();
		dxcMacroDefine.push_back(dxcMacroMember);
	}
	if (mShaderMacros.size() > 0)
	{
		dxcMacroDefinePointer = dxcMacroDefine.data();
	}
	hres = sDxcCompiler->Compile(
		sourceBlob.Get(), // pSource
		L"PS.hlsl", // pSourceName
		shaderEntry, // pEntryPoint
		shaderStageName, // pTargetProfile
		arguments.data(), (uint32_t)arguments.size(), // pArguments, argCount
		dxcMacroDefinePointer, dxcMacroDefine.size(), // pDefines, defineCount
		&includeHandler, // pIncludeHandler
		&result); // ppResult
	LUNA_ASSERT(SUCCEEDED(hres));
	result->GetResult(&code);
	ComPtr<IDxcBlobEncoding> error;
	result->GetErrorBuffer(error.GetAddressOf());
	LString str((char*)error->GetBufferPointer(), (char*)error->GetBufferPointer() + error->GetBufferSize());
	
	result->GetStatus(&hres);
	if (FAILED(hres))
		LogError("Render", str.c_str());
	LUNA_ASSERT(SUCCEEDED(hres));
	spirv.resize(code->GetBufferSize() / 4);
	memcpy(spirv.data(), code->GetBufferPointer(), code->GetBufferSize());
	return true;
}

HRESULT STDMETHODCALLTYPE LunaIncludeHandler::LoadSource(_In_z_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource)
{
	HRESULT hr;
	ComPtr<IDxcBlobEncoding> pEncoding;
	LString path = WstringToString(pFilename);
	if (IncludedFiles.find(path) != IncludedFiles.end())
	{
		// Return empty string blob if this file has been included before
		static const char nullStr[] = " ";
		sDxcLib->CreateBlobWithEncodingOnHeapCopy("", 0, CP_UTF8, pEncoding.GetAddressOf());
		*ppIncludeSource = pEncoding.Detach();
		return S_OK;
	}
	LString context = sAssetModule->LoadAsset<TextAsset>("assets/built-in/Shader" + path)->GetContent();
	uint32_t codePage = CP_UTF8;
	hr = sDxcLib->CreateBlobWithEncodingOnHeapCopy(context.c_str(), (uint32_t)context.Length(), codePage, &pEncoding);
	ComPtr<IDxcOperationResult> result;
	if (SUCCEEDED(hr))
	{
		IncludedFiles.insert(path);
		*ppIncludeSource = pEncoding.Detach();
	}
	else
	{
		*ppIncludeSource = nullptr;
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE LunaIncludeHandler::QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject)
{
	return sDefaultIncludeHandler->QueryInterface(riid, ppvObject);
}

}