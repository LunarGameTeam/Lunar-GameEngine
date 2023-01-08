#include "core/reflection/reflection.h"

#include "core/binding/binding.h"
#include "core/binding/binding_module.h"

#include "core/math/math.h"
#include "core/math/math_binding.h"


#include "imgui.h"
#include "imgui_internal.h"

#include "render/renderer/imgui_texture.h"

#include "editor/ui/icon_font.h"


namespace luna
{


inline LVector2f ToVector2(const ImVec2& val)
{
	return LVector2f(val.x, val.y);
}

inline ImVec2 ToVec2(const LVector2f& val)
{
	return ImVec2(val.x(), val.y());
}

inline LVector4f ToVector4(const ImVec4& val)
{
	return LVector4f(val.x, val.y, val.z, val.w);
}


inline ImVec4 ToVec4(const LVector4f& val)
{
	return ImVec4(val.x(), val.y(), val.z(), val.w());
}

}

namespace luna::binding
{


template<>
struct binding_converter<ImVec2>
{
	inline static PyObject* to_binding(const ImVec2& val)
	{		
		return binding_converter<LVector2f>::to_binding(ToVector2(val));
	}

	inline static ImVec2 from_binding(PyObject* val)
	{		
		return ToVec2(binding_converter<LVector2f>::from_binding(val));
	}

	static const char* binding_fullname()
	{
		return "luna.LVector2f";
	}
};

template<>
struct binding_converter<ImVec4>
{
	inline static PyObject* to_binding(const ImVec4& val)
	{
		return binding_converter<LVector4f>::to_binding(ToVector4(val));
	}

	inline static ImVec4 from_binding(PyObject* val)
	{
		return ToVec4(binding_converter<LVector4f>::from_binding(val));
	}

	static const char* binding_fullname()
	{
		return "luna.LVector4f";
	}
};

template<>
struct binding_converter<const ImGuiPayload*>
{
	inline static PyObject* to_binding(const ImGuiPayload* val)
	{
		if (!val)
			Py_RETURN_NONE;
		PyObject* data = *(PyObject**)(val->Data);
		PyObject* res = PyDict_New();
		PyDict_SetItemString(res, "data", data);
		PyDict_SetItemString(res, "type", binding_converter<const char*>::to_binding(val->DataType));
		return res;
	}

	inline static const ImGuiPayload* from_binding(PyObject* val)
	{
		return nullptr;
	}

	static const char* binding_fullname()
	{
		return "dict";
	}
};



}
namespace luna
{

struct InputTextCallback_UserData
{
	LString Str;
	ImGuiInputTextCallback ChainCallback;
};

int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		LString& str = user_data->Str;
		IM_ASSERT(data->Buf == str.c_str());
		str.std_str().resize(data->BufTextLen);
		data->Buf = (char*)str.c_str();
	}
	return 0;
}


PyObject* InputLString(const char* id,const LString& str, ImGuiInputTextFlags flags)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;
	InputTextCallback_UserData data;
	data.Str = str;
	bool changed = ImGui::InputText(id, (char*)data.Str.c_str(), data.Str.std_str().size() + 1, flags, InputTextCallback, &data);

	PyObject* ret_args = PyTuple_New(2);
	PyObject* new_val = changed ? to_binding(data.Str) : Py_NewRef(Py_None);
	PyTuple_SetItem(ret_args, 0, to_binding(changed));
	PyTuple_SetItem(ret_args, 1, new_val);
	return ret_args;
}

static bool PyMenuItem(const char* label) { return ImGui::MenuItem(label); }

static void PyText(const char* text) { return ImGui::Text(text); }

static bool PyButton(const char* text) { return ImGui::Button(text); }

static void PyPushID(void* id){ return ImGui::PushID(id); }

static bool PyTreeNode(void* id, ImGuiTreeNodeFlags flags, const char* name) 
{
	return ImGui::TreeNodeEx(id, flags, name); 
}

static PyObject* PyDragFloat3(const char* label, const LVector3f& val, float speed, float v_min, float v_max)
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

static PyObject* PyDragFloat(const char* label, float val, float speed, float v_min, float v_max)
{
	float float_val = val;
	bool res = ImGui::DragFloat(label, &float_val, speed, v_min, v_max);

	PyObject* ret_args = PyTuple_New(2);
	PyObject* new_val = to_binding(float_val);
	PyTuple_SetItem(ret_args, 0, to_binding(res));
	PyTuple_SetItem(ret_args, 1, new_val);
	return ret_args;
}

static PyObject* PyCheckBox(const char* label, bool value)
{
	bool modify = ImGui::Checkbox(label, &value);

	PyObject* ret = PyTuple_New(2);
	PyObject* newVal = to_binding(value);
	PyTuple_SetItem(ret, 0, to_binding(modify));
	PyTuple_SetItem(ret, 1, newVal);
	return ret;
}

LVector2f PyCalcTextSize(const char* val)
{
	LVector2f res;
	auto vec = ImGui::CalcTextSize(val);
	res.x() = vec.x;
	res.y() = vec.y;
	return res;
}

bool TreeNodeCallbackEx(void* ptr_id, ImGuiTreeNodeFlags flag, std::function<void(bool, bool)> func)
{
	ImGuiIO& io = ImGui::GetIO();

	

	ImGuiID id = ImGui::GetID(ptr_id);
	ImGuiWindow* window = ImGui::GetCurrentContext()->CurrentWindow;
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
		ImGui::Text(" ");
	}
	ImGui::SameLine(x + 16);
	ImGui::ItemAdd(bb, id);
	func(hovered, clicked);
	if (opened)
		ImGui::TreePush(ptr_id);
	return opened;
}

void PyImage(render::ImguiTexture* texture, const LVector2f& size, const LVector2f& uv0, const LVector2f& uv1)
{
	ImGui::Image((ImTextureID)texture->mImg, ToVec2(size), ToVec2(uv0), ToVec2(uv1));
}

bool PySetDragDropPayload(const char* type, PyObject* data, ImGuiCond cond)
{
	return ImGui::SetDragDropPayload(type, &data, sizeof(PyObject*), cond);
}

const ImGuiPayload* PyAcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags /* = 0 */)
{
	const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type, flags);
	return payload;	
}

#define AddIMGUIConstant(name) imguiModule->AddConstant(#name, name);

STATIC_INIT(imgui)
{
		BindingModule* imguiModule = BindingModule::Get("luna.imgui");

		AddIMGUIConstant(ImGuiCond_Always);
		AddIMGUIConstant(ImGuiCond_FirstUseEver);
		AddIMGUIConstant(ImGuiCond_Once);

		AddIMGUIConstant(ImGuiDragDropFlags_None);
		AddIMGUIConstant(ImGuiDragDropFlags_AcceptBeforeDelivery);		

		AddIMGUIConstant(ImGuiPopupFlags_None);
		AddIMGUIConstant(ImGuiPopupFlags_MouseButtonRight);

		AddIMGUIConstant(ImGuiTreeNodeFlags_DefaultOpen);
		AddIMGUIConstant(ImGuiTreeNodeFlags_Leaf);
		AddIMGUIConstant(ImGuiTreeNodeFlags_OpenOnArrow);
		AddIMGUIConstant(ImGuiTreeNodeFlags_OpenOnDoubleClick);

		AddIMGUIConstant(ICON_FA_COMPRESS);
		AddIMGUIConstant(ICON_FA_CUBE);
		AddIMGUIConstant(ICON_FA_FOLDER);
		AddIMGUIConstant(ICON_FA_FILE);
		AddIMGUIConstant(ICON_FA_IMAGE);
		AddIMGUIConstant(ICON_FA_LAYER_GROUP);

		imguiModule->AddMethod<&InputLString>("input");
		imguiModule->AddMethod<[](){
			ImGui::ShowDemoWindow(nullptr);
		}>("show_demo_window");

		imguiModule->AddMethod<&ImGui::GetWindowContentRegionMin> ("get_window_content_min");
		imguiModule->AddMethod<&ImGui::GetWindowContentRegionMax>("get_window_content_max");
		
		imguiModule->AddMethod<&ImGui::IsMouseHoveringRect>("is_mouse_hovering_rect");
		imguiModule->AddMethod<&ImGui::GetMouseDragDelta>("get_mouse_drag_delta");
		imguiModule->AddMethod<&ImGui::ResetMouseDragDelta>("reset_mouse_drag_delta");

		imguiModule->AddMethod<&ImGui::BeginPopupContextItem>("begin_popup_context_item");
		imguiModule->AddMethod<&ImGui::EndPopup>("end_popup");

		imguiModule->AddMethod<&ImGui::BeginMenuBar>("begin_menu_bar");
		imguiModule->AddMethod<&ImGui::EndMenuBar>("end_menu_bar");
		imguiModule->AddMethod<&ImGui::BeginMenu>("begin_menu");
		imguiModule->AddMethod<&ImGui::EndMenu>("end_menu");
		imguiModule->AddMethod<&ImGui::GetWindowPos>("get_window_pos");
		imguiModule->AddMethod<&PyMenuItem>("menu_item");
		imguiModule->AddMethod<&PyImage>("image");

		imguiModule->AddMethod<&ImGui::BeginGroup>("begin_group");
		imguiModule->AddMethod<&ImGui::EndGroup>("end_group");
		
		imguiModule->AddMethod<&PyTreeNode>("tree_node");
		imguiModule->AddMethod<&TreeNodeCallbackEx>("tree_node_callback");

		imguiModule->AddMethod<&ImGui::TreePop>("tree_pop");
		imguiModule->AddMethod<&PyPushID>("push_id");
		imguiModule->AddMethod<&ImGui::PopID>("pop_id");
		imguiModule->AddMethod<&ImGui::SameLine>("same_line");
		imguiModule->AddMethod<&PyText>("text");
		imguiModule->AddMethod<&PyButton>("button");
		
		imguiModule->AddMethod<&PyCalcTextSize>("calc_text_size");
		imguiModule->AddMethod<&ImGui::GetContentRegionAvail>("get_content_region_avail");


		imguiModule->AddMethod<&PyDragFloat3>("drag_float3");
		imguiModule->AddMethod<&PyDragFloat>("drag_float");
		imguiModule->AddMethod<&PyCheckBox>("checkbox");

		imguiModule->AddMethod<&ImGui::BeginDragDropSource>("begin_drag_drop_souce");
		imguiModule->AddMethod<&ImGui::EndDragDropSource>("end_drag_drop_souce");
		imguiModule->AddMethod<&ImGui::BeginDragDropTarget>("begin_drag_drop_target");
		imguiModule->AddMethod<&ImGui::EndDragDropTarget>("end_drag_drop_target");
		imguiModule->AddMethod<&ImGui::AcceptDragDropPayload>("accept_drag_drop_payload");
				
		imguiModule->AddMethod<&PySetDragDropPayload>("set_drag_drop_payload");


		imguiModule->AddMethod<&ImGui::AlignTextToFramePadding>("align_text_to_frame_padding");
		imguiModule->AddMethod<&ImGui::Separator>("separator");

		imguiModule->Init();

}

}
