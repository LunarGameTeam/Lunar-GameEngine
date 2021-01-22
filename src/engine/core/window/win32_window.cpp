#include "win32_window.h"

#include "window_subsystem.h"
#include <ShellScalingApi.h>

#pragma comment(lib, "Shcore.lib")

namespace luna
{

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowSubsystem *subsytem = gEngine->GetSubsystem<WindowSubsystem>();
	LWindow *window = subsytem->GetWindowByHandle(hwnd);

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool LWin32Window::Init()
{
	HINSTANCE appInstance = GetModuleHandle(NULL);
	m_appInstance = appInstance;
	WNDCLASSEX wndClass;
	memset(&wndClass, 0, sizeof(wndClass));
	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(wndClass);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = appInstance;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = m_window_name.c_str();
	wndClass.lpszMenuName = nullptr;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	//取消dpi对游戏的缩放
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	if (!RegisterClassEx(&wndClass))
	{
		MessageBox(NULL, TEXT("RegisterClassEx function failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	int screenWidth, screenHeight;
	int posX, posY;
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	//真实的windows窗口大小(窗口模式下，窗口大小大于渲染窗口的大小)
	int real_window_width;
	int real_window_height;
	if (m_full_screen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPanningWidth = screenWidth;
		dmScreenSettings.dmPanningWidth = screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		m_width = screenWidth;
		m_height = screenHeight;
		real_window_width = m_width;
		real_window_height = m_height;
		posX = posY = 0;
	}
	else
	{
		posX = (screenWidth - m_width) / 2;
		posY = (screenHeight - m_height) / 2;
		//获取渲染窗口真正的大小
		RECT R = { 0, 0, m_width, m_height };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		real_window_width = R.right - R.left;
		real_window_height = R.bottom - R.top;
	}

	mHWND = CreateWindowEx(
		WS_EX_APPWINDOW,
		wndClass.lpszClassName,
		wndClass.lpszClassName,
		WS_OVERLAPPEDWINDOW,
		posX, posY,
		real_window_width, real_window_height,
		NULL,
		NULL,
		appInstance,
		nullptr);

	if (!mHWND)
	{
		MessageBox(NULL, TEXT("CreateWindowEx function failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(mHWND, SW_SHOW);
	UpdateWindow(mHWND);
	SetFocus(mHWND);

	ShowCursor(true);

	return true;
}
}
