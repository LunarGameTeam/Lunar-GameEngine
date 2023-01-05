#include "main_panel.h"
#include "core/event/event_module.h"
#include "engine/scene_module.h"
#include "core/asset/asset_module.h"

#include "editor/ui/icon_font.h"
#include "imstb_textedit.h"


namespace luna::editor
{

RegisterTypeEmbedd_Imp(MainPanel)
{
	cls->Ctor<MainPanel>();
	cls->Binding<MainPanel>();
	BindingModule::Get("luna.editor")->AddType(cls);
};


void MainPanel::OnInputEvent(LWindow& window, InputEvent& event)
{
}

void MainPanel::OnWindowResize(LWindow& window, WindowEvent& evt)
{
	mWidth = evt.width;
	mHeight = evt.height;
}


void MainPanel::Init()
{
	auto func = std::bind(&MainPanel::OnInputEvent, this, std::placeholders::_1, std::placeholders::_2);
	static auto handle = sEventModule->OnInput.Bind(func);
	auto resize_func = std::bind(&MainPanel::OnWindowResize, this, std::placeholders::_1, std::placeholders::_2);
	sWindowModule->OnWindowResize.Bind(resize_func);
	mTitle =  LString::Format(" {0} Luna Engine Editor", ICON_FA_MOON);
}

void MainPanel::DoIMGUI()
{
	WindowModule* window = gEngine->GetModule<WindowModule>();
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
	ImGui::SetNextWindowSize(ImVec2((float)main_window->GetWindowWidth(), (float)main_window->GetWindowHeight()));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
	ImGui::Begin(mTitle.c_str(), &open,
				 ImGuiWindowFlags_MenuBar |
				 ImGuiWindowFlags_NoBringToFrontOnFocus |
				 ImGuiWindowFlags_NoMove |
				 ImGuiWindowFlags_NoCollapse);

	ImGuiContext* context = ImGui::GetCurrentContext();
	InvokeBinding("on_imgui");
	ImGui::PopStyleVar();
	dockspaceID = ImGui::GetID("MainEditor");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f),
	                 ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);

	if ((moveOffsetY >= 0 && moveOffsetY <= 22) && // Cursor must be *on* the titlebar
		ImGui::IsMouseDragging(0)) // User must drag mouse (hold LMB)
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
}
