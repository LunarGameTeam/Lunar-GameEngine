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
#include "core/config/config.h"
#include "window/window_subsystem.h"
#include "window/imgui_impl_sdl.h"
#include "window/imgui_impl_dx11.h"


namespace luna
{
namespace editor
{

class EditorBase
{
public:
	virtual void OnGUI();
};

class MainEditor : public EditorBase
{
public:
	int moveOffsetX = 0;
	int moveOffsetY = 0;
	bool open = true;
	void OnGUI()
	{
		WindowSubsystem* window = gEngine->GetSubsystem<WindowSubsystem>();
		auto main_window = window->GetMainWindow();
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(main_window->GetWindow(), &wmInfo);
		HWND hwnd = (HWND)wmInfo.info.win.window;
		if (ImGui::IsMouseClicked(0))
		{
			POINT point;
			RECT rect;

			GetCursorPos(&point);
			GetWindowRect(hwnd, &rect);

			// Calculate the difference between the cursor pos and window pos
			moveOffsetX = point.x - rect.left;
			moveOffsetY = point.y - rect.top;
		}


		static ImGuiID dockspaceID = 0;
		bool active = true;
		ImGui::SetNextWindowSize(ImVec2(luna::DefaultWidth.GetValue(), luna::DefaultHeight.GetValue()));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Luna Engine", &open, ImGuiWindowFlags_MenuBar);
		ImGui::BeginMenuBar();
		ImGui::EndMenuBar();
		dockspaceID = ImGui::GetID("HUB_DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode/*|ImGuiDockNodeFlags_NoResize*/);

		if ((moveOffsetY >= 0 && moveOffsetY <= 22) &&	// Cursor must be *on* the titlebar
			ImGui::IsMouseDragging(0))					// User must drag mouse (hold LMB)
		{
			POINT point;
			GetCursorPos(&point);
			SetWindowPos(hwnd, nullptr, point.x - moveOffsetX, point.y - moveOffsetY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}

		ImGui::End();
		ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
		if (!open)
			gEngine->SetPendingExit(!open);
	}
};

class SceneEditor : public EditorBase
{
public:
	void OnGUI()
	{
		ImGui::Begin("Scene", nullptr);
		ImGui::End();
	}
};

class InspectorEditor : public EditorBase
{
public:
	void OnGUI()
	{
		ImGui::Begin("Inspector", nullptr);
		ImGui::End();
	}
};

class LibraryEditor : public EditorBase
{
public:
	void OnGUI()
	{

		ImGui::Begin("Library", nullptr);
		ImGui::End();

	}
};

class HierarchyEditor : public EditorBase
{
public:
	void OnGUI()
	{
		ImGui::Begin("Hierarchy", nullptr);
		ImGui::End();
	}
};

}
}