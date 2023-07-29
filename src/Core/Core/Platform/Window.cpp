#include "Core/Platform/Window.h"

#include "imgui_impl_sdl2.h"
#include "Core/Event/EventModule.h"

#include <d3d11.h>
#include <windows.h>
#include <psapi.h>

namespace luna
{

RegisterTypeEmbedd_Imp(LWindow)
{

	cls->Binding<LWindow>();
	cls->BindingProperty<&LWindow::mWidth>("width");
	cls->BindingProperty<&LWindow::mHeight>("height");
	cls->BindingMethod<&LWindow::GetWindowX>("get_window_x");
	cls->BindingMethod<&LWindow::GetWindowY>("get_window_y");
	cls->BindingMethod<&LWindow::SetWindowPos>("set_window_pos");
	cls->BindingMethod<&LWindow::GetMousePos>("get_mouse_pos");

	BindingModule::Luna()->AddType(cls);
}



LWindow::LWindow(int32_t width , int32_t heght):
	mWidth(width),
	mHeight(heght)
{
}

bool LWindow::Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}
	
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS);
	
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

int LWindow::GetWindowX()
{
	int x, y;
	SDL_GetWindowPosition(mSDLWindow, &x, &y);
	return x;
}

int LWindow::GetWindowY()
{
	int x, y;
	SDL_GetWindowPosition(mSDLWindow, &x, &y);
	return y;
}

void LWindow::SetWindowPos(int x, int y)
{
	SDL_SetWindowPosition(mSDLWindow, x, y);
}

luna::LVector2f LWindow::GetMousePos()
{
	int x,  y;
	SDL_GetMouseState(&x, &y);
	int windowX, windowY;
	SDL_GetWindowPosition(mSDLWindow, &windowX, &windowY);
	return LVector2f(windowX + x, windowY + y);
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
}

uint32_t LWindow::Id()
{
	return SDL_GetWindowID(mSDLWindow);
}

}