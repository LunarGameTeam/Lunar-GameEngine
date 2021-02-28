#include "window_subsystem.h"
#include "boost/make_shared.hpp"
#include "core/core_module.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include "core/event/event_subsystem.h"

#include <d3d11.h>
#include <windows.h>
#include <psapi.h>

#include <tchar.h>

namespace luna
{

CONFIG_IMPLEMENT(int, Window, UsingImGUI, 1);
CONFIG_IMPLEMENT(int, Window, DefaultWidth, 1024);
CONFIG_IMPLEMENT(int, Window, DefaultHeight, 768);

LWindow *WindowSubsystem::CreateLunaWindow(const luna::LString &name, int width, int height)
{

#ifdef _WIN32
	auto win32Window = MakeShared<LWindow>();
	win32Window->Init();
	m_win_windows[win32Window->Id()] = win32Window;
#endif // _WIN32
	

	return win32Window.get();
}

LWindow *WindowSubsystem::GetMainWindow()
{
	return m_main_window;
}

bool WindowSubsystem::OnPreInit()
{
// 	ImGui::CreateContext();
// 	IMGUI_CHECKVERSION();
// 	ImGuiIO& io = ImGui::GetIO();(void)io;
// 	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
 	m_main_window = CreateLunaWindow("MainWindow", 1024, 768);
// 	ImGui::StyleColorsLight();


// 	ImGuiStyle* style = &ImGui::GetStyle();
// 
// 	style->WindowPadding = ImVec2(15, 15);
// 	style->WindowRounding = 5.0f;
// 	style->FramePadding = ImVec2(5, 5);
// 	style->FrameRounding = 4.0f;
// 	style->ItemSpacing = ImVec2(12, 8);
// 	style->ItemInnerSpacing = ImVec2(8, 6);
// 	style->IndentSpacing = 25.0f;
// 	style->ScrollbarSize = 15.0f;
// 	style->ScrollbarRounding = 9.0f;
// 	style->GrabMinSize = 5.0f;
// 	style->GrabRounding = 3.0f;
// 
// 	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
// 	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
// 	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
// 	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
// 	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
// 	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
// 	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
// 	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
// 	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
// 	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
// 	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
// 	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
// 	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
// 	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
// 	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
// 	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
// 	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
// 	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
// 	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
// 	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
// 	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
// 	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
// 	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
// 	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
// 	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
// 	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
// 	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
// 	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
// 	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
// 	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
// 	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
// 	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
// 	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
// 	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);

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

void WindowSubsystem::Tick(float delta_time)
{
	static EventSubsystem* event_subsystem = gEngine->GetSubsystem<EventSubsystem>();
	SDL_Event event;
	bool done = true;
	while (SDL_PollEvent(&event))
	{
		//ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			gEngine->SetPendingExit(true);
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
			gEngine->SetPendingExit(true);
		auto window = m_win_windows[event.window.windowID];
		switch (event.type)

		{
		case SDL_KEYDOWN:
		{
			break;
		}
		case SDL_KEYUP:
		{
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			InputEvent input;
			input.type = EventType::Input_MousePress;
			if (event.button.button == SDL_BUTTON_LEFT)
				input.code = KeyCode::MouseLeft;
			if (event.button.button == SDL_BUTTON_RIGHT)
				input.code = KeyCode::MouseRight;
			input.x = event.button.x;
			input.y = event.button.y;
			event_subsystem->OnInput.BroadCast(*window, input);
			break;
		}
		default:
			break;
		}
	}
	if (m_main_window && !gEngine->GetPendingExit())
	{
		if (!m_main_window->Tick())
			m_main_window->OnDestroy();

		m_main_window->OnPreGUI();
		gEngine->OnSubsystemIMGUI();
		m_main_window->OnPostGUI();
	}
	
}

}