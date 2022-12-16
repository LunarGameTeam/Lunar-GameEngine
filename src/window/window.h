/*!
 * \file lunar_window.h
 * \date 2020/08/09 11:16
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#pragma once
#include "window/window_config.h"

#include "core/core_library.h"


#include <SDL.h>
#include <SDL_syswm.h>


namespace luna
{

class WINDOW_API LWindow
{
public:
	using WindowHandle = Uint32;

	LWindow();
	virtual bool Init();


	inline int32_t GetWindowWidth()
	{
		return mWidth;
	}
	inline int32_t GetWindowHeight()
	{
		return mHeight;
	}
	HWND GetWin32HWND();

	bool Tick();
	void OnDestroy();

	WindowHandle Id();
	GETTER(SDL_Window*, mSDLWindow, Window);
protected:
	SDL_Window *mSDLWindow;
	bool mFullScreen = false;
	luna::LString mWindowName = "LunarGame-Engine";
	int32_t mWidth = 1024;
	int32_t mHeight = 768;
};


}