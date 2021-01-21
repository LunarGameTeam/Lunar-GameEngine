#include "window_subsystem.h"
#include "boost/make_shared.hpp"
#include "core/core_module.h"
namespace luna
{


LWindow *WindowSubsystem::CreateLunaWindow(const luna::LString &name, int width, int height)
{

#ifdef _WIN32 || _WIN64
	LWin32Window *win32Window = new LWin32Window();
#endif // _WIN32 || _WIN64

	win32Window->Init();
	auto hwnd = win32Window->GetHwnd();
	m_win_windows[hwnd] = win32Window;
	return win32Window;
}

LWindow *WindowSubsystem::GetMainWindow()
{
	return m_main_window;
}

bool WindowSubsystem::OnPreInit()
{
	m_main_window = CreateLunaWindow("MainWindow", 1024, 768);
	//("[Window]MainWindow Created");
	return true;
}

bool WindowSubsystem::OnInit()
{
	m_need_tick = true;
	return true;

}

bool WindowSubsystem::OnPostInit()
{
	return true;
}

bool WindowSubsystem::OnShutdown()
{
	return true;
}

void WindowSubsystem::Tick()
{

	MSG msg;
	memset(&msg, 0, sizeof(msg));
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (WM_QUIT == msg.message)
		gEngine->mPendingExit = true;
}


}