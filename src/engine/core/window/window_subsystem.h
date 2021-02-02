/*!
 * \file window_subsystem.h
 * \date 2020/08/09 11:14
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
#include "core/misc/container.h"
#include "luna_window.h"

namespace luna
{

class CORE_API WindowSubsystem : public SubSystem
{
#ifdef _WIN32
	using WindowHandle = HWND;
#endif // _WIN32

public:

	LWindow *CreateLunaWindow(const luna::LString &name, int width, int height);
	LWindow *GetMainWindow();
	LWindow *GetWindowByHandle(WindowHandle handle)
	{
		return m_win_windows[handle];
	}


public:
	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

private:
	LMap<WindowHandle, LWindow *> m_win_windows;
	LWindow *m_main_window;
};

}