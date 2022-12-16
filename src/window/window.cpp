#include "window.h"
#include "window_module.h"

#include "imgui_impl_sdl.h"
#include "core/event/event_module.h"

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

LWindow::LWindow()
{
}

bool LWindow::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}
	
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN);
	
	mSDLWindow = SDL_CreateWindow("Luna Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight, window_flags);
	if (mSDLWindow == nullptr)
	{
		printf("Error: %s\n", SDL_GetError());
	}
	SDL_SysWMinfo wmInfo;
 	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(mSDLWindow, &wmInfo);
	HWND hwnd = (HWND)wmInfo.info.win.window;


	// Setup Platform/Renderer backends
	return true;
}

HWND LWindow::GetWin32HWND()
{
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(mSDLWindow, &wmInfo);
	HWND hwnd = (HWND)wmInfo.info.win.window;
	return hwnd;
}

bool LWindow::Tick()
{
	return true;
}

void LWindow::OnDestroy()
{
	// Release all outstanding references to the swap chain's buffers before resizing.
	g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
}

luna::LWindow::WindowHandle LWindow::Id()
{
	return SDL_GetWindowID(mSDLWindow);
}

}