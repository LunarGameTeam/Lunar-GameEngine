#include "render_subsystem.h"
#include "window/window_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include <d3dcompiler.h>
#include "CommonStates.h"
#include "interface/i_camera.h"

#include "d3d11/d3d11_device.h"
#include "d3d11/shader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

namespace luna
{
namespace legacy_render{

RenderSubusystem::RenderSubusystem()
{
	m_need_tick = true;
}

bool RenderSubusystem::OnPreInit()
{
	return true;
}

bool RenderSubusystem::OnPostInit()
{
	return true;
}

HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob **blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob *shaderBlob = nullptr;
	ID3DBlob *errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
									entryPoint, profile,
									flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char *)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

bool RenderSubusystem::OnInit()
{
	static AssetSubsystem *asset_sys = gEngine->GetSubsystem<AssetSubsystem>();

	m_deviceResources = std::make_unique<DX::DeviceResources>(
		DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT, 2,
		D3D_FEATURE_LEVEL_9_1);
	m_deviceResources->RegisterDeviceNotify(this);
	LWindow *main_window = gEngine->GetSubsystem<luna::WindowSubsystem>()->GetMainWindow();
	auto window = main_window->GetWindow();
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = (HWND)wmInfo.info.win.window;
	m_deviceResources->SetWindow(hwnd, main_window->GetWindowWidth(), main_window->GetWindowHeight());
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();
	bool ret = true;
	shader = new Dx11Shader();
	ret = shader->Init();
	if (!ret)
	{
		return false;
	}
	model = asset_sys->LoadAsset<Mesh>("/assets/box.fbx");
	/*ret = model->Init();*/
	if (!ret)
	{
		return false;
	}
	return true;
}

bool RenderSubusystem::OnShutdown()
{
	return true;
}

void RenderSubusystem::Tick(float delta_time)
{	
	Clear();
	auto context = m_deviceResources->GetD3DDeviceContext();
	if (m_main_camera)
	{
		shader->Bind();
		//shader->SetWVPMatrix(model->GetWolrdMatrix(), m_main_camera->GetViewMatrix(), m_main_camera->GetProjectionMatrix());
// 		model->Bind();
// 		model->Draw();
	}
	// Show the new frame.
	m_deviceResources->Present();
}

void RenderSubusystem::OnDeviceLost()
{

}

void RenderSubusystem::OnDeviceRestored()
{

}

void RenderSubusystem::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}

}
}


