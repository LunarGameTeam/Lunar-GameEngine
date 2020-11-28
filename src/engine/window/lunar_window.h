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

#include "core/core_module.h"
#include <windows.h>
#include <windowsx.h>

class LunarWindow
{
public:
	virtual bool Init() = 0;
	inline int32_t GetWindowWidth() 
	{
		return mWidth;
	}
	inline int32_t GetWindowHeight()
	{
		return mHeight;
	}
protected:
	bool mFullScreen = false;
	LunarEngine::LString mWindowName = "LunarGame-Engine";
	int32_t mWidth = 1024;
	int32_t mHeight = 768;
};

class LunarWin32Window : public LunarWindow
{
public:
	virtual bool Init();
	HWND GetHwnd() 
	{
		return mHWND;
	}
	HINSTANCE GetInstanceHwnd()
	{
		return m_appInstance;
	}
private:
	HWND mHWND;
	HINSTANCE m_appInstance;
};