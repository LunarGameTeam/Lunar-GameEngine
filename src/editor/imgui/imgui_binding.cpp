#include "imstb_textedit.h"

#include "editor/ui/ImGuiFileDialog.h"

#include "core/reflection/reflection.h"

#include "core/binding/binding.h"
#include "core/binding/binding_module.h"

#include "core/math/math.h"
#include "core/math/math_binding.h"


#include "imgui.h"
#include "editor/ui/icon_font.h"


namespace luna
{

LVector2f ToVector2(const ImVec2& val) 
{
	return LVector2f(val.x, val.y);
}

ImVec2 FromVector2(const LVector2f& val)
{
	return ImVec2(val.x(), val.y());
}

static bool MenuItem(const char* label) { return ImGui::MenuItem(label); }
static void Text(const char* text) { return ImGui::Text(text); }
static bool Button(const char* text) { return ImGui::Button(text); }
static void PushID(int id)
{
	return ImGui::PushID(id);
}
static void SameLine(float offset, float spacing) 
{ 
	return ImGui::SameLine(offset, spacing); 
}
static bool TreeNode(const char* id, ImGuiTreeNodeFlags flags, const char* name) 
{
	return ImGui::TreeNodeEx(id, flags, name); 
}

static PyObject* DragFloat3(const char* label, const LVector3f& val, float speed, float v_min, float v_max)
{
	float float_val[3];
	float_val[0] = val.x();
	float_val[1] = val.y();
	float_val[2] = val.z();
	bool res = ImGui::DragFloat3(label, float_val, speed, v_min, v_max);

	PyObject* ret_args = PyTuple_New(2);	
	PyObject* new_val = res ? to_binding(LVector3f(float_val[0], float_val[1], float_val[2])) : Py_NewRef(Py_None);
	PyTuple_SetItem(ret_args, 0, to_binding(res));	
	PyTuple_SetItem(ret_args, 1, new_val);
	return ret_args;
}

static PyObject* DragFloat(const char* label, float val, float speed, float v_min, float v_max)
{
	float float_val = val;
	bool res = ImGui::DragFloat(label, &float_val, speed, v_min, v_max);

	PyObject* ret_args = PyTuple_New(2);
	PyObject* new_val = to_binding(float_val);
	PyTuple_SetItem(ret_args, 0, to_binding(res));
	PyTuple_SetItem(ret_args, 1, new_val);
	return ret_args;
}

static PyObject* CheckBox(const char* label, bool value)
{
	bool modify = ImGui::Checkbox(label, &value);

	PyObject* ret = PyTuple_New(2);
	PyObject* newVal = to_binding(value);
	PyTuple_SetItem(ret, 0, to_binding(modify));
	PyTuple_SetItem(ret, 1, newVal);
	return ret;
}

LVector2f CalcTextSize(const char* val)
{
	LVector2f res;
	auto vec = ImGui::CalcTextSize(val);
	res.x() = vec.x;
	res.y() = vec.y;
	return res;
}

STATIC_INIT(imgui)
{
		BindingModule* imguiModule = BindingModule::Get("luna.imgui");
		
		imguiModule->AddConstant("ImGuiTreeNodeFlags_DefaultOpen", ImGuiTreeNodeFlags_DefaultOpen);
		imguiModule->AddConstant("ImGuiTreeNodeFlags_Leaf", ImGuiTreeNodeFlags_Leaf);

		imguiModule->AddConstant("ICON_FA_COMPRESS", ICON_FA_COMPRESS);
		imguiModule->AddConstant("ICON_FA_CUBE", ICON_FA_CUBE);

		
		imguiModule->AddMethod<&ImGui::BeginMenuBar>("begin_menu_bar");
		imguiModule->AddMethod<&ImGui::EndMenuBar>("end_menu_bar");
		imguiModule->AddMethod<&ImGui::BeginMenu>("begin_menu");
		imguiModule->AddMethod<&ImGui::EndMenu>("end_menu");
		imguiModule->AddMethod<&ImGui::BeginGroup>("begin_group");
		imguiModule->AddMethod<&ImGui::EndGroup>("end_group");
		imguiModule->AddMethod<&TreeNode>("tree_node");
		imguiModule->AddMethod<&ImGui::TreePop>("tree_pop");
		imguiModule->AddMethod<&PushID>("push_id");
		imguiModule->AddMethod<&ImGui::PopID>("pop_id");
		imguiModule->AddMethod<&SameLine>("same_line");
		imguiModule->AddMethod<&MenuItem>("menu_item");
		imguiModule->AddMethod<&Text>("text");
		imguiModule->AddMethod<&Button>("button");
		imguiModule->AddMethod<&CalcTextSize>("calc_text_size");


		imguiModule->AddMethod<&DragFloat3>("drag_float3");
		imguiModule->AddMethod<&DragFloat>("drag_float");
		imguiModule->AddMethod<&CheckBox>("checkbox");
		imguiModule->Init();

}

}
