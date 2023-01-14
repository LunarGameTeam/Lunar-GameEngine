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



#include "core/reflection/reflection.h"


#include "core/binding/binding.h"


#include <SDL.h>
#include <SDL_syswm.h>


namespace luna
{

class CORE_API LWindow
{
	RegisterTypeEmbedd(LWindow, InvalidType);
public:

	LWindow(int32_t width = 1024, int32_t heght = 768);
	virtual bool Init();


	inline int32_t GetWindowWidth()
	{
		return mWidth;
	}
	inline int32_t GetWindowHeight()
	{
		return mHeight;
	}

	int GetWindowX();
	int GetWindowY();

	void SetWindowPos(int x, int y);
	LVector2f GetMousePos();

	HWND GetWin32HWND();

	bool Tick();
	void OnDestroy();

	uint32_t Id();
	GETTER(SDL_Window*, mSDLWindow, Window);
protected:
	SDL_Window *mSDLWindow;
	bool mFullScreen = false;
	luna::LString mWindowName = "LunarGame-Engine";
	int32_t mWidth = 1024;
	int32_t mHeight = 768;
};


namespace binding
{

template<> struct binding_converter<LWindow*> : native_converter<LWindow> { };
template<> struct binding_proxy<LWindow> : native_binding_proxy<LWindow> { };

}



}