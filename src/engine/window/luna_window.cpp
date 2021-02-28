#include "luna_window.h"
#include "window_subsystem.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include "core/event/event_subsystem.h"

#include <d3d11.h>
#include <windows.h>
#include <psapi.h>

namespace luna
{
// Data
static ID3D11Device *g_pd3dDevice = NULL;
static ID3D11DeviceContext *g_pd3dDeviceContext = NULL;
static IDXGISwapChain *g_pSwapChain = NULL;
static ID3D11RenderTargetView *g_mainRenderTargetView = NULL;

LWindow::WindowHandle window_id = 0;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

LWindow::LWindow()
{
}

bool LWindow::Init()
{
	
	// Setup SDL
   // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
   // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	
	m_window = SDL_CreateWindow("Luna Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, window_flags);
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(m_window, &wmInfo);
	HWND hwnd = (HWND)wmInfo.info.win.window;

// 	// Initialize Direct3D
// 	if (!CreateDeviceD3D(hwnd))
// 	{
// 		CleanupDeviceD3D();
// 		return 1;
// 	}
// 
// 	// Setup Dear ImGui style
// 	ImGui::StyleColorsDark();
// 	//ImGui::StyleColorsClassic();
// 
// 	// Setup Platform/Renderer backends
// 	ImGui_ImplSDL2_InitForD3D(m_window);
// 	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
}

bool LWindow::Tick()
{
	return true;
}

void LWindow::OnPreGUI()
{
// 	// Start the Dear ImGui frame
// 	ImGui_ImplDX11_NewFrame();
// 	ImGui_ImplSDL2_NewFrame(m_window);
// 	ImGui::NewFrame();
}

void LWindow::OnPostGUI()
{
// 	ImVec4 clear_color = ImVec4(1.f, 1.f, 1.f, 1.00f);
// 	// Rendering
// 	ImGui::Render();
// 	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
// 	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
// 	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
// 	g_pSwapChain->Present(1, 0); // Present with vsync
}

void LWindow::OnDestroy()
{
	// Release all outstanding references to the swap chain's buffers before resizing.
	CleanupRenderTarget();
	g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	CreateRenderTarget();
}

luna::LWindow::WindowHandle LWindow::Id()
{
	return SDL_GetWindowID(m_window);
}

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

// 	UINT createDeviceFlags = 0;
// 	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
// 	D3D_FEATURE_LEVEL featureLevel;
// 	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
// 	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
// 		return false;
// 
// 	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D *pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}


}