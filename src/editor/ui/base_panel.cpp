#include "editor/ui/base_panel.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "icon_font.h"


namespace luna::editor
{


RegisterTypeEmbedd_Imp(PanelBase)
{	
	cls->Ctor<PanelBase>();
	cls->BindingMethod<&PanelBase::OnGUI>("on_imgui");
	cls->BindingProperty<&PanelBase::mTitle>("title");
	cls->BindingProperty<&PanelBase::mHasMenu>("has_menubar");
	cls->Binding<PanelBase>();
	BindingModule::Get("luna.editor")->AddType(cls);
};

void PanelBase::DoIMGUI()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
	if (mHasMenu)
		flags = flags | ImGuiWindowFlags_MenuBar;
	ImGui::Begin(mTitle.c_str(), nullptr, flags);
	mFocus = ImGui::IsWindowFocused();
	ImVec2 vec2 = ImGui::GetWindowSize();
	mWidth = vec2.x;
	mHeight = vec2.y;

	InvokeBinding("on_imgui");
	
	
	ImGui::End();
}

void PanelBase::OnGUI()
{

}

}
