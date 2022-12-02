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
#include "pch.h"
#include "core/core_library.h"
#include "core/misc/container.h"
#include "core/config/config.h"
#include "core/event/EventModule.h"
#include "window.h"

namespace luna
{
WINDOW_API CONFIG_DECLARE(int, Window, UsingImGUI, 1);
WINDOW_API CONFIG_DECLARE(int, Window, DefaultWidth, 1024);
WINDOW_API CONFIG_DECLARE(int, Window, DefaultHeight, 768);

class WINDOW_API WindowModule : public LModule
{
	RegisterTypeEmbedd(WindowModule, LModule)
public:
	SIGNAL(OnWindowResize, LWindow &, WindowEvent &);

	WindowModule();;
	LWindow *CreateLunaWindow(const luna::LString &name, int width, int height);
	LWindow *GetMainWindow();
	LWindow *GetWindowByHandle(LWindow::WindowHandle handle)
	{
		return mWindows[handle].get();
	}




	void OnIMGUI() override;

	void ImGUINewFrame();

public:
	bool OnLoad() override;

	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

private:
	LMap<LWindow::WindowHandle, LSharedPtr<LWindow>> mWindows;
	LWindow *mMainWindow;
	
};
WINDOW_API extern WindowModule *sWindowModule;
}