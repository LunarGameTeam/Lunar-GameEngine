#include "editor/ui/base_panel.h"
#include "imgui_internal.h"
#include "icon_font.h"


namespace luna::editor
{


RegisterTypeEmbedd_Imp(PanelBase)
{	
	cls->Ctor<PanelBase>();
	cls->BindingMethod<&PanelBase::OnGUI>("on_imgui");
	cls->BindingMethod<&PanelBase::CustomTreeNode>("custom_treenode");
	cls->BindingProperty<&PanelBase::mTitle>("title");
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

bool PanelBase::CustomTreeNode(const char* label, ImGuiTreeNodeFlags flag, std::function<void(bool, bool)> func)
{
	ImGuiIO& io = ImGui::GetIO();

	ImGuiWindow* window = ImGui::GetCurrentWindow();

	ImGuiID id = window->GetID(label);
	ImVec2 pos = window->DC.CursorPos;
	ImRect bb(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::GetFontSize()));
	bool opened = ImGui::TreeNodeBehaviorIsOpen(id, flag);
	bool hovered, held;
	bool clicked = false;

	if (ImGui::ButtonBehavior(bb, id, &hovered, &held, true))
	{
		clicked = true;
		window->DC.StateStorage->SetInt(id, opened ? 0 : 1);
	}
	if (hovered || held || (flag & ImGuiTreeNodeFlags_Selected))
		window->DrawList->AddRectFilled(bb.Min, bb.Max,
		                                ImGui::GetColorU32(held ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered));


	float x = ImGui::GetCursorPosX();
	if (!(flag & ImGuiTreeNodeFlags_Leaf))
	{
		ImGui::Text(opened ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);
	}
	else
	{
		ImGui::Text("");
	}
	ImGui::SameLine(x + 10);
	ImGui::ItemAdd(bb, id);
	func(hovered, clicked);
	if (opened)
		ImGui::TreePush(label);
	return opened;
}

void PanelBase::OnGUI()
{
}
}
