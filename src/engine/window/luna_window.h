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
#include "private_window.h"
#include "core/core_module.h"
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
		return m_width;
	}
	inline int32_t GetWindowHeight()
	{
		return m_height;
	}
	bool Tick();
	void OnPreGUI();
	void OnPostGUI();
	void OnDestroy();

	WindowHandle Id();
	GETTER(SDL_Window*, m_window, Window);
protected:
	SDL_Window *m_window;
	bool m_full_screen = false;
	luna::LString m_window_name = "LunarGame-Engine";
	int32_t m_width = 1024;
	int32_t m_height = 768;
};


}