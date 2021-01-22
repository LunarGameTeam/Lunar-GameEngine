#pragma once

#include "luna_window.h"
#include <windows.h>
#include <windowsx.h>

namespace luna
{

class LWin32Window : public LWindow
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

}