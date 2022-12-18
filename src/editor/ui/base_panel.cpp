#include "editor/ui/base_panel.h"
#include "imgui_internal.h"
#include "icon_font.h"


namespace luna::editor
{


RegisterTypeEmbedd_Imp(EditorBase)
{	
	cls->Ctor<EditorBase>();
	cls->BindingMethod<&EditorBase::OnGUI>("on_imgui");
	cls->BindingMethod<&EditorBase::CustomTreeNode>("custom_treenode");
	cls->Binding<EditorBase>();
	BindingModule::Get("luna.editor")->AddType(cls);
};

void EditorBase::DoIMGUI()
{
	ImGui::Begin(mTitle.c_str(), nullptr, ImGuiWindowFlags_MenuBar);
	mFocus = ImGui::IsWindowFocused();
	ImVec2 vec2 = ImGui::GetWindowSize();
	mWidth = vec2.x;
	mHeight = vec2.y;

	InvokeBinding("on_imgui");
	
	
	ImGui::End();
}

int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		LString* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->str().resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	return 0;
}

bool EditorBase::CustomTreeNode(const char* label, ImGuiTreeNodeFlags flag, std::function<void(bool, bool)> func)
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

void EditorBase::OnGUI()
{
}

bool InputLString(const char* label, LString* str, ImGuiInputTextFlags flags)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = InputTextCallback;
	return ImGui::InputText(label, (char*)str->c_str(), str->str().capacity() + 1, flags, InputTextCallback,
	                        &cb_user_data);
}
}
