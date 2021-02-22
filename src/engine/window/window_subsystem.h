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
#include "private_window.h"
#include "core/core_module.h"
#include "core/misc/container.h"
#include "core/config/config.h"
#include "luna_window.h"


namespace luna
{

WINDOW_API CONFIG_DECLARE(int, Window, UsingImGUI, 1);
WINDOW_API CONFIG_DECLARE(int, Window, DefaultWidth, 1024);
WINDOW_API CONFIG_DECLARE(int, Window, DefaultHeight, 768);

class WINDOW_API WindowSubsystem : public SubSystem
{
public:
	WindowSubsystem() {};
	LWindow *CreateLunaWindow(const luna::LString &name, int width, int height);
	LWindow *GetMainWindow();
	LWindow *GetWindowByHandle(LWindow::WindowHandle handle)
	{
		return m_win_windows[handle].get();
	}


public:
	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

private:	
	LMap<LWindow::WindowHandle, LSharedPtr<LWindow>> m_win_windows;
	LWindow *m_main_window;
};

}