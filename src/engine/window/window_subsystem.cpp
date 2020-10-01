#include "window_subsystem.h"
#include "boost/make_shared.hpp"
#include "core/core_module.h"

Ptr<LunarWindow> WindowSubusystem::CreateLunarWindow(const LunarEngine::LString& name, int width, int height)
{

#ifdef _WIN32 || _WIN64
	Ptr<LunarWindow> win32Window(CreateObject<LunarWin32Window>(this).get());
#endif // _WIN32 || _WIN64

	win32Window->Init();
	return win32Window;
}

Ptr<LunarWindow> WindowSubusystem::GetMainWindow()
{
	return mMainWindow;
}

bool WindowSubusystem::OnPreInit()
{
	mMainWindow = CreateLunarWindow("LunarGameEngine", 1024, 768);
	//("[Window]MainWindow Created");
	return true;
}

bool WindowSubusystem::OnInit()
{
	mNeedTick = true;
	return true;

}

bool WindowSubusystem::OnPostInit()
{
	return true;
}

bool WindowSubusystem::OnShutdown()
{
	return true;
}

void WindowSubusystem::Tick()
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

