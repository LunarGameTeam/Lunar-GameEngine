#include "Core/Reflection/Reflection.h"

#include "Core/Scripting/Binding.h"
#include "Core/Scripting/BindingModule.h"

#include "Core/Math/Math.h"
#include "Core/Math/MathBinding.h"


#include "imgui.h"
#include "imgui_internal.h"

#include "Core/Foundation/AwesomeFont.h"

#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RenderModule.h"

#include "ImGuizmo.h"


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
struct binding_converter<ImGuiKey>
{
	inline static PyObject* to_binding(ImGuiKey val)
	{
		return binding_converter<int>::to_binding(int(val));
	}

	inline static ImGuiKey from_binding(PyObject* val)
	{
		return (ImGuiKey)(binding_converter<int>::from_binding(val));
	}

	static const char* binding_fullname()
	{
		return "int";
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

void PySetDrawList()
{
	ImGuizmo::SetDrawlist(nullptr);
}

void PyDrawGrid(const LMatrix4f& view, const LMatrix4f& projection, const LMatrix4f& matrix, float size)
{
	LMatrix4f tView = view.transpose();
	LMatrix4f tProjection = projection.transpose();
	LMatrix4f tObject = matrix.transpose();
	ImGuizmo::DrawGrid(tView.data(), tProjection.data(), tObject.data(), size);
}

void PyDrawCube(const LMatrix4f& view, const LMatrix4f& projection, const LMatrix4f& matrix, int count)
{
	LMatrix4f tView = view.transpose();
	LMatrix4f tProjection = projection.transpose();
	LMatrix4f tObject = matrix.transpose();
	ImGuizmo::DrawCubes(tView.data(), tProjection.data(), tObject.data(), count);
}

PyObject* PyManipulate(const LMatrix4f& view, const LMatrix4f& projection, int op, int mode,
                       const LMatrix4f& obj)
{
	LMatrix4f tView = view.transpose();
	LMatrix4f tProjection = projection.transpose();
	LMatrix4f tObject = obj.transpose();

	bool handled = ImGuizmo::Manipulate(tView.data(), tProjection.data(), ImGuizmo::OPERATION(op), ImGuizmo::MODE(mode),
	                                    tObject.data());
	LMatrix4f result = tObject.transpose();
	return binding_return(handled, result);
}

bool PyIsOver(int op)
{
	return ImGuizmo::IsOver(ImGuizmo::OPERATION(op));
}


bool PySelectable(const char* label, bool selected, int flags, const ImVec2& size)
{
	bool res = ImGui::Selectable(label, &selected, flags, size);
	return binding_return(res, selected);
}

#define AddIMGUIConstant(name) imguiModule->AddConstant(#name, name)

STATIC_INIT(imgui)
{
	BindingModule* gizmosModule = BindingModule::Get("luna.imgui.gizmos");
	gizmosModule->AddMethod<&ImGuizmo::SetRect>("set_rect");
	gizmosModule->AddMethod<&ImGuizmo::SetOrthographic>("set_orthographic");
	gizmosModule->AddMethod<&ImGuizmo::BeginFrame>("begin_frame");
	gizmosModule->AddMethod<&ImGuizmo::IsUsing>("is_using");
	gizmosModule->AddMethod<&PyIsOver>("is_over");
	gizmosModule->AddMethod<&PySetDrawList>("set_draw_list");
	gizmosModule->AddMethod<&PyDrawGrid>("draw_grid");
	gizmosModule->AddMethod<&PyDrawCube>("draw_cubes");
	gizmosModule->AddMethod<&PyManipulate>("manipulate");
	gizmosModule->AddConstant("Mode_Local", ImGuizmo::MODE::LOCAL);
	gizmosModule->AddConstant("Mode_WORLD", ImGuizmo::MODE::WORLD);
	gizmosModule->AddConstant("Operation_BOUNDS", ImGuizmo::OPERATION::BOUNDS);
	gizmosModule->AddConstant("Operation_TRANSLATE", ImGuizmo::OPERATION::TRANSLATE);
	gizmosModule->AddConstant("Operation_ROTATE", ImGuizmo::OPERATION::ROTATE);
	gizmosModule->AddConstant("Operation_SCALE", ImGuizmo::OPERATION::SCALE);

	BindingModule* imguiModule = BindingModule::Get("luna.imgui");


	imguiModule->AddLambda<[](const char* id, const LString& str, ImGuiInputTextFlags flags)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		flags |= ImGuiInputTextFlags_CallbackResize;
		InputTextCallback_UserData data;
		data.Str = str;
		bool changed = ImGui::InputText(id, (char*)data.Str.c_str(), data.Str.std_str().size() + 1, flags,
		                                InputTextCallback, &data);

		return binding_return(changed, changed ? to_binding(data.Str) : Py_NewRef(Py_None));
	}>("input");

	imguiModule->AddLambda<[]()
	{
		ImGui::ShowDemoWindow(nullptr);
	}>("show_demo_window");

	imguiModule->AddLambda<[](const char* name, ImGuiWindowFlags flags, bool showClose)
	{
		bool b = true;
		if (showClose)
		{
			ImGui::Begin(name, &b, flags);
		}
		else
		{
			ImGui::Begin(name, nullptr, flags);
		}
		if (showClose)
			return b;
		return true;
	}>("begin");

	imguiModule->AddMethod<&ImGui::End>("end");

	imguiModule->AddMethod<(bool(*)(const char*, const ImVec2&, bool, ImGuiWindowFlags))&ImGui::BeginChild>(
		"begin_child");
	imguiModule->AddMethod<&ImGui::EndChild>("end_child");


	imguiModule->AddLambda<[](const ImVec2& min, const ImVec2& max)
	{
		ImGui::SetNextWindowSizeConstraints(min, max);
	}>("set_next_window_size_constraints");

	imguiModule->AddMethod<&ImGui::GetWindowContentRegionMin>("get_window_content_min");
	imguiModule->AddMethod<&ImGui::GetWindowContentRegionMax>("get_window_content_max");

	imguiModule->AddMethod<&ImGui::IsMouseHoveringRect>("is_mouse_hovering_rect");
	imguiModule->AddMethod<&ImGui::IsItemHovered>("is_item_hovered");
	imguiModule->AddMethod<&ImGui::IsMouseDragging>("is_mouse_dragging");
	imguiModule->AddMethod<&ImGui::IsMouseDoubleClicked>("is_mouse_double_clicked");

	imguiModule->AddMethod<(bool(*)(ImGuiMouseButton))&ImGui::IsMouseDown>("is_mouse_down");
	imguiModule->AddMethod<&ImGui::GetMouseDragDelta>("get_mouse_drag_delta");
	imguiModule->AddMethod<&ImGui::ResetMouseDragDelta>("reset_mouse_drag_delta");

	imguiModule->AddMethod<&ImGui::BeginPopupContextItem>("begin_popup_context_item");
	imguiModule->AddMethod<&ImGui::BeginPopup>("begin_popup");
	imguiModule->AddMethod<(void(*)(const char*, int))&ImGui::OpenPopup>("open_popup");
	imguiModule->AddMethod<&ImGui::EndPopup>("end_popup");
	imguiModule->AddLambda<[](const char* name, bool p_open, ImGuiWindowFlags flags /* = 0 */)
	{
		PyObject* ret_args = PyTuple_New(2);
		bool show = ImGui::BeginPopupModal(name, &p_open, flags);
		return binding_return(show, p_open);
	}>("begin_popup_modal");

	imguiModule->AddMethod<&ImGui::BeginMenuBar>("begin_menu_bar");

	imguiModule->AddMethod<&ImGui::EndMenuBar>("end_menu_bar");
	imguiModule->AddMethod<&ImGui::BeginMenu>("begin_menu");
	imguiModule->AddMethod<&ImGui::EndMenu>("end_menu");
	imguiModule->AddMethod<&ImGui::GetWindowPos>("get_window_pos");

	imguiModule->AddLambda<[](const char* label) -> bool
	{
		return ImGui::MenuItem(label);
	}>("menu_item");

	imguiModule->AddLambda<[](const char* label) -> bool
	{
		return ImGui::MenuItem(label);
	}>("menu_item");

	imguiModule->AddLambda<[](render::RHIResource* texture, const LVector2f& size, const LVector2f& uv0,
	                          const LVector2f& uv1)
	{
		if (!sRenderModule->IsImuiTexture(texture))
			sRenderModule->AddImguiTexture(texture);
		ImGui::Image((ImTextureID)sRenderModule->GetImguiTexture(texture)->mImg, ToVec2(size), ToVec2(uv0),
		             ToVec2(uv1));
	}>("image");

	imguiModule->AddMethod<&ImGui::BeginGroup>("begin_group");
	imguiModule->AddMethod<&ImGui::EndGroup>("end_group");

	imguiModule->AddLambda<[](uint64_t id, ImGuiTreeNodeFlags flags, const char* name)-> bool
	{
		return ImGui::TreeNodeEx(reinterpret_cast<const void*>(id), flags, name);
	}>("tree_node");

	imguiModule->AddLambda<[](uint64_t pid, const ImVec2& size,ImGuiTreeNodeFlags flag)-> PyObject*
	{
		auto treeID = reinterpret_cast<const void*>(pid);
		ImGuiID id = ImGui::GetID(treeID);

		ImGuiWindow* window = ImGui::GetCurrentContext()->CurrentWindow;
		ImVec2 pos = window->DC.CursorPos;
		float width = size.x > 0 ? size.x : ImGui::GetContentRegionAvail().x;
		float height = size.y > 0 ? size.y : ImGui::GetFontSize() + 4;

		
		ImRect arrowRect(pos, ImVec2(pos.x + 16, pos.y + height));
		ImRect itemRect(pos, ImVec2(pos.x + width, pos.y + height));

		
		bool expand = ImGui::TreeNodeBehaviorIsOpen(id, flag);
		bool hoverd, held, double_click = false;
		bool clicked = false;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (height - ImGui::GetFontSize()) * 0.5f);
		if ((flag & ImGuiTreeNodeFlags_Leaf))
		{
			ImGui::Text(" ");
		}
		else
		{
			ImGui::Text(expand ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT);
			if (ImGui::ButtonBehavior(arrowRect, id, nullptr, nullptr))
			{
				expand = !expand;			
				window->DC.StateStorage->SetInt(id, expand);
			}
		}

		float x = ImGui::GetCursorPosX();

		ImGui::SameLine(x + 16);
		
		ImGui::ItemAdd(itemRect, id);
		if (ImGui::IsItemClicked(0))
		{
			clicked = true;
			if(ImGui::IsMouseDoubleClicked(0))
			{
				double_click = true;
				expand = !expand;
				window->DC.StateStorage->SetInt(id, expand);
			}			
		}

		hoverd = ImGui::IsItemHovered();

		if (hoverd)
			window->DrawList->AddRectFilled(itemRect.Min, itemRect.Max, ImGui::GetColorU32(ImGuiCol_DockingPreview));


		if (expand)
			ImGui::TreePush(treeID);

		return binding_return(clicked, expand, double_click);
	}>("tree_node_callback");

	imguiModule->AddMethod<&ImGui::IsMouseDragging>("is_mouse_dragging");
	imguiModule->AddMethod<static_cast<bool(*)(ImGuiMouseButton)>(&ImGui::IsMouseReleased)>("is_mouse_released");
	imguiModule->AddMethod<static_cast<bool(*)(ImGuiMouseButton, bool)>(&ImGui::IsMouseClicked)>("is_mouse_clicked");

	imguiModule->AddMethod<static_cast<bool(*)(ImGuiKey)>(&ImGui::IsKeyDown)>("is_key_down");
	imguiModule->AddMethod<static_cast<bool(*)(ImGuiKey, bool)>(&ImGui::IsKeyPressed)>("is_key_pressed");
	imguiModule->AddMethod<static_cast<bool(*)(ImGuiKey)>(&ImGui::IsKeyReleased)>("is_key_released");

	imguiModule->AddMethod<&ImGui::TreePop>("tree_pop");

	imguiModule->AddLambda<[](void* id)
	{
		return ImGui::PushID(id);
	}>("push_id");

	imguiModule->AddMethod<&ImGui::PopID>("pop_id");
	imguiModule->AddMethod<&ImGui::SameLine>("same_line");

	imguiModule->AddLambda<[](const char* text)
	{
		ImGui::Text(text);
	}>("text");
	imguiModule->AddLambda<[](const ImVec4 &color, const char* text)
	{
		ImGui::TextColored(color, text);
	} > ("text_colored");
	imguiModule->AddLambda<[](const char* label, bool selected, int flags, const ImVec2& size)
	{
		bool res = ImGui::Selectable(label, &selected, flags, size);
		return binding_return(res, selected);
	}>("selectable");
	imguiModule->AddLambda<[](const char* text) { return ImGui::Button(text); }>("button");
	imguiModule->AddLambda<[]
	(const char* id, render::RHIResource* image, const ImVec2& size, const ImVec2& uv, const ImVec2& uv2,
	 const ImVec4& bgCol, const ImVec4& hintCol)
		{
			if (!sRenderModule->IsImuiTexture(image))
				sRenderModule->AddImguiTexture(image);
			return ImGui::ImageButton(id, (ImTextureID)sRenderModule->GetImguiTexture(image)->mImg, size, uv, uv2,
			                          bgCol, hintCol);
		}
	>("image_button");

	imguiModule->AddLambda<[](const char* val)
	{
		LVector2f res;
		auto vec = ImGui::CalcTextSize(val);
		res.x() = vec.x;
		res.y() = vec.y;
		return res;
	}>("calc_text_size");
	imguiModule->AddMethod<&ImGui::GetContentRegionAvail>("get_content_region_avail");


	imguiModule->AddLambda<[](const char* label, const LVector3f& val, float speed, float v_min, float v_max)
	{
		float float_val[3];
		float_val[0] = val.x();
		float_val[1] = val.y();
		float_val[2] = val.z();
		bool res = ImGui::DragFloat3(label, float_val, speed, v_min, v_max);

		return binding_return(res, res
			                           ? to_binding(LVector3f(float_val[0], float_val[1], float_val[2]))
			                           : Py_NewRef(Py_None));
	}>("drag_float3");

	imguiModule->AddLambda<[](const char* label, float val, float speed, float v_min, float v_max)
	{
		float float_val = val;
		bool res = ImGui::DragFloat(label, &float_val, speed, v_min, v_max);

		return binding_return(res, float_val);
	}>("drag_float");

	imguiModule->AddLambda<[](const char* label, bool value)
	{
		bool modify = ImGui::Checkbox(label, &value);
		return binding_return(modify, value);
	}>("checkbox");

	imguiModule->AddLambda<[](const char* label, const LVector4f& val, ImGuiColorEditFlags flags)
	{
		float float_val[4];
		float_val[0] = val.x();
		float_val[1] = val.y();
		float_val[2] = val.z();
		float_val[3] = val.w();
		bool res = ImGui::ColorPicker4(label, float_val, flags);

		return binding_return(res, res
			                           ? to_binding(LVector4f(float_val[0], float_val[1], float_val[2], float_val[3]))
			                           : Py_NewRef(Py_None));
	}>("color4_picker");

	imguiModule->AddMethod<&ImGui::BeginDragDropSource>("begin_drag_drop_souce");
	imguiModule->AddMethod<&ImGui::EndDragDropSource>("end_drag_drop_souce");
	imguiModule->AddMethod<&ImGui::BeginDragDropTarget>("begin_drag_drop_target");
	imguiModule->AddMethod<&ImGui::EndDragDropTarget>("end_drag_drop_target");
	imguiModule->AddMethod<&ImGui::AcceptDragDropPayload>("accept_drag_drop_payload");


	imguiModule->AddLambda<[](const char* type, PyObject* data, ImGuiCond cond)
		{
			return ImGui::SetDragDropPayload(type, &data, sizeof(PyObject*), cond);
		}
	>("set_drag_drop_payload");


	imguiModule->AddMethod<&ImGui::AlignTextToFramePadding>("align_text_to_frame_padding");
	imguiModule->AddMethod<&ImGui::Separator>("separator");

	imguiModule->AddMethod<&ImGui::SetNextWindowSize>("set_next_window_size");
	imguiModule->AddMethod<&ImGui::SetNextWindowPos>("set_next_window_pos");
	imguiModule->AddLambda<[]()
	{
		return ImGui::GetMainViewport()->ID;
	}>("get_main_viewport_id");
	imguiModule->AddLambda<[](ImGuiID id)
	{
		auto viewport = ImGui::FindViewportByID(id);
		if (viewport)
			return viewport->Pos;
		return ImVec2(0, 0);
	}>("get_viewport_pos");

	imguiModule->AddLambda<[]()
		{
			return ImGui::GetWindowViewport()->ID;
		}
	>("get_window_viewport");
	imguiModule->AddMethod<&ImGui::SetNextWindowDockID>("set_next_window_dock_id");
	imguiModule->AddMethod<&ImGui::SetNextWindowViewport>("set_next_window_viewport");
	imguiModule->AddMethod<&ImGui::GetWindowSize>("get_window_size");
	imguiModule->AddLambda<[](ImGuiID id, const ImVec2& size /* = ImVec2(0, 0) */, ImGuiDockNodeFlags flags /* = 0 */)
	{
		ImGui::DockSpace(id, size, flags);
	}>("dock_space");

	imguiModule->AddMethod<&ImGui::GetCursorPos>("get_cursor_pos");
	imguiModule->AddMethod<&ImGui::SetCursorPos>("set_cursor_pos");
	imguiModule->AddMethod<&ImGui::GetMousePos>("get_mouse_pos");

	imguiModule->AddMethod<(void(*)(int, float))&ImGui::PushStyleVar>("push_style_float");
	imguiModule->AddMethod<(void(*)(int, const ImVec4&))&ImGui::PushStyleColor>("push_style_color");
	imguiModule->AddMethod<(void(*)(int, const ImVec2&))&ImGui::PushStyleVar>("push_style_vec2");
	imguiModule->AddMethod<&ImGui::PopStyleVar>("pop_style_var");
	imguiModule->AddMethod<&ImGui::PopStyleColor>("pop_style_color");
	imguiModule->AddMethod<&ImGui::GetStyleColorVec4>("get_style_color");

	imguiModule->AddLambda<[](int col, const ImVec4& val)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[col] = val;
	}>("set_color");

	imguiModule->AddLambda<[](int col, uint32_t val)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[col] = ImGui::ColorConvertU32ToFloat4(val);
	}>("set_color_uint");

	imguiModule->AddLambda<[](const char* val)
	{
		return ImGui::GetID(val);
	}>("get_id");

	AddIMGUIConstant(ImGuiSelectableFlags_None);
	AddIMGUIConstant(ImGuiSelectableFlags_SelectOnClick);
	//Cond
	AddIMGUIConstant(ImGuiCond_Always);
	AddIMGUIConstant(ImGuiCond_FirstUseEver);
	AddIMGUIConstant(ImGuiCond_Once);

	AddIMGUIConstant(ImGuiDockNodeFlags_PassthruCentralNode);
	AddIMGUIConstant(ImGuiDockNodeFlags_AutoHideTabBar);
	//Window Flags
	AddIMGUIConstant(ImGuiWindowFlags_NoTitleBar);
	AddIMGUIConstant(ImGuiWindowFlags_NoResize);
	AddIMGUIConstant(ImGuiWindowFlags_NoMove);
	AddIMGUIConstant(ImGuiWindowFlags_NoScrollbar);
	AddIMGUIConstant(ImGuiWindowFlags_NoScrollWithMouse);
	AddIMGUIConstant(ImGuiWindowFlags_NoCollapse);
	AddIMGUIConstant(ImGuiWindowFlags_AlwaysAutoResize);
	AddIMGUIConstant(ImGuiWindowFlags_NoBackground);
	AddIMGUIConstant(ImGuiWindowFlags_NoSavedSettings);
	AddIMGUIConstant(ImGuiWindowFlags_NoMouseInputs);
	AddIMGUIConstant(ImGuiWindowFlags_MenuBar);
	AddIMGUIConstant(ImGuiWindowFlags_HorizontalScrollbar);
	AddIMGUIConstant(ImGuiWindowFlags_NoFocusOnAppearing);
	AddIMGUIConstant(ImGuiWindowFlags_NoBringToFrontOnFocus);
	AddIMGUIConstant(ImGuiWindowFlags_AlwaysVerticalScrollbar);
	AddIMGUIConstant(ImGuiWindowFlags_AlwaysHorizontalScrollbar);
	AddIMGUIConstant(ImGuiWindowFlags_AlwaysUseWindowPadding);
	AddIMGUIConstant(ImGuiWindowFlags_NoNavInputs);
	AddIMGUIConstant(ImGuiWindowFlags_NoNavFocus);
	AddIMGUIConstant(ImGuiWindowFlags_UnsavedDocument);
	AddIMGUIConstant(ImGuiWindowFlags_NoDocking);
	AddIMGUIConstant(ImGuiWindowFlags_NoNav);
	AddIMGUIConstant(ImGuiWindowFlags_NoDecoration);
	AddIMGUIConstant(ImGuiWindowFlags_NoInputs);

	//DragDrop
	AddIMGUIConstant(ImGuiDragDropFlags_None);
	AddIMGUIConstant(ImGuiDragDropFlags_AcceptBeforeDelivery);
	AddIMGUIConstant(ImGuiDragDropFlags_SourceAllowNullID);


	//Popup Flags
	AddIMGUIConstant(ImGuiPopupFlags_None);
	AddIMGUIConstant(ImGuiPopupFlags_MouseButtonRight);

	//TreeNode Flags
	AddIMGUIConstant(ImGuiTreeNodeFlags_DefaultOpen);
	AddIMGUIConstant(ImGuiTreeNodeFlags_Leaf);
	AddIMGUIConstant(ImGuiTreeNodeFlags_OpenOnArrow);
	AddIMGUIConstant(ImGuiTreeNodeFlags_CollapsingHeader);

	AddIMGUIConstant(ImGuiTreeNodeFlags_OpenOnDoubleClick);

	AddIMGUIConstant(ImGuiColorEditFlags_None);
	AddIMGUIConstant(ImGuiColorEditFlags_NoAlpha);
	AddIMGUIConstant(ImGuiColorEditFlags_NoPicker);
	AddIMGUIConstant(ImGuiColorEditFlags_NoOptions);
	AddIMGUIConstant(ImGuiColorEditFlags_NoSmallPreview);
	AddIMGUIConstant(ImGuiColorEditFlags_NoInputs);
	AddIMGUIConstant(ImGuiColorEditFlags_NoTooltip);
	AddIMGUIConstant(ImGuiColorEditFlags_NoLabel);
	AddIMGUIConstant(ImGuiColorEditFlags_NoSidePreview);
	AddIMGUIConstant(ImGuiColorEditFlags_NoDragDrop);
	AddIMGUIConstant(ImGuiColorEditFlags_NoBorder);

	AddIMGUIConstant(ImGuiColorEditFlags_AlphaBar);
	AddIMGUIConstant(ImGuiColorEditFlags_AlphaPreview);
	AddIMGUIConstant(ImGuiColorEditFlags_AlphaPreviewHalf);
	AddIMGUIConstant(ImGuiColorEditFlags_HDR);
	AddIMGUIConstant(ImGuiColorEditFlags_DisplayRGB);
	AddIMGUIConstant(ImGuiColorEditFlags_DisplayHSV);
	AddIMGUIConstant(ImGuiColorEditFlags_DisplayHex);
	AddIMGUIConstant(ImGuiColorEditFlags_Uint8);
	AddIMGUIConstant(ImGuiColorEditFlags_Float);
	AddIMGUIConstant(ImGuiColorEditFlags_PickerHueWheel);
	AddIMGUIConstant(ImGuiColorEditFlags_PickerHueBar);
	AddIMGUIConstant(ImGuiColorEditFlags_InputRGB);
	AddIMGUIConstant(ImGuiColorEditFlags_InputHSV);


	//Key
	AddIMGUIConstant(ImGuiKey_A);
	AddIMGUIConstant(ImGuiKey_B);
	AddIMGUIConstant(ImGuiKey_C);
	AddIMGUIConstant(ImGuiKey_D);
	AddIMGUIConstant(ImGuiKey_E);
	AddIMGUIConstant(ImGuiKey_F);
	AddIMGUIConstant(ImGuiKey_G);
	AddIMGUIConstant(ImGuiKey_H);
	AddIMGUIConstant(ImGuiKey_I);
	AddIMGUIConstant(ImGuiKey_J);
	AddIMGUIConstant(ImGuiKey_K);
	AddIMGUIConstant(ImGuiKey_L);
	AddIMGUIConstant(ImGuiKey_M);
	AddIMGUIConstant(ImGuiKey_N);
	AddIMGUIConstant(ImGuiKey_O);
	AddIMGUIConstant(ImGuiKey_P);
	AddIMGUIConstant(ImGuiKey_Q);
	AddIMGUIConstant(ImGuiKey_R);
	AddIMGUIConstant(ImGuiKey_S);
	AddIMGUIConstant(ImGuiKey_T);
	AddIMGUIConstant(ImGuiKey_U);
	AddIMGUIConstant(ImGuiKey_V);
	AddIMGUIConstant(ImGuiKey_W);
	AddIMGUIConstant(ImGuiKey_X);
	AddIMGUIConstant(ImGuiKey_Y);
	AddIMGUIConstant(ImGuiKey_Z);

	AddIMGUIConstant(ImGuiKey_Space);
	AddIMGUIConstant(ImGuiKey_Delete);
	AddIMGUIConstant(ImGuiKey_Enter);
	AddIMGUIConstant(ImGuiKey_0);
	AddIMGUIConstant(ImGuiKey_1);
	AddIMGUIConstant(ImGuiKey_2);
	AddIMGUIConstant(ImGuiKey_3);
	AddIMGUIConstant(ImGuiKey_4);
	AddIMGUIConstant(ImGuiKey_5);
	AddIMGUIConstant(ImGuiKey_6);
	AddIMGUIConstant(ImGuiKey_7);
	AddIMGUIConstant(ImGuiKey_8);
	AddIMGUIConstant(ImGuiKey_9);

	AddIMGUIConstant(ImGuiHoveredFlags_None);

	AddIMGUIConstant(ImGuiStyleVar_Alpha);
	AddIMGUIConstant(ImGuiStyleVar_DisabledAlpha);
	AddIMGUIConstant(ImGuiStyleVar_WindowRounding);
	AddIMGUIConstant(ImGuiStyleVar_WindowPadding);
	AddIMGUIConstant(ImGuiStyleVar_WindowBorderSize);
	AddIMGUIConstant(ImGuiStyleVar_WindowMinSize);
	AddIMGUIConstant(ImGuiStyleVar_WindowTitleAlign);
	AddIMGUIConstant(ImGuiStyleVar_ChildBorderSize);
	AddIMGUIConstant(ImGuiStyleVar_PopupRounding);
	AddIMGUIConstant(ImGuiStyleVar_PopupBorderSize);
	AddIMGUIConstant(ImGuiStyleVar_FramePadding);
	AddIMGUIConstant(ImGuiStyleVar_FrameRounding);
	AddIMGUIConstant(ImGuiStyleVar_FrameBorderSize);
	AddIMGUIConstant(ImGuiStyleVar_ItemSpacing); // ImVec2    ItemSpacing
	AddIMGUIConstant(ImGuiStyleVar_ItemInnerSpacing); // ImVec2    ItemInnerSpacing
	AddIMGUIConstant(ImGuiStyleVar_IndentSpacing); // float     IndentSpacing
	AddIMGUIConstant(ImGuiStyleVar_CellPadding); // ImVec2    CellPadding
	AddIMGUIConstant(ImGuiStyleVar_ScrollbarSize); // float     ScrollbarSize
	AddIMGUIConstant(ImGuiStyleVar_ScrollbarRounding); // float     ScrollbarRounding
	AddIMGUIConstant(ImGuiStyleVar_GrabMinSize); // float     GrabMinSize
	AddIMGUIConstant(ImGuiStyleVar_GrabRounding); // float     GrabRounding
	AddIMGUIConstant(ImGuiStyleVar_TabRounding); // float     TabRounding
	AddIMGUIConstant(ImGuiStyleVar_ButtonTextAlign); // ImVec2    ButtonTextAlign
	AddIMGUIConstant(ImGuiStyleVar_SelectableTextAlign); // ImVec2    SelectableTextAlign


	AddIMGUIConstant(ImGuiCol_Text);
	AddIMGUIConstant(ImGuiCol_TextDisabled);
	AddIMGUIConstant(ImGuiCol_WindowBg); // Background of normal windows
	AddIMGUIConstant(ImGuiCol_ChildBg); // Background of child windows
	AddIMGUIConstant(ImGuiCol_PopupBg); // Background of popups, menus, tooltips windows
	AddIMGUIConstant(ImGuiCol_Border);
	AddIMGUIConstant(ImGuiCol_BorderShadow);
	AddIMGUIConstant(ImGuiCol_FrameBg); // Background of checkbox, radio button, plot, slider, text input
	AddIMGUIConstant(ImGuiCol_FrameBgHovered);
	AddIMGUIConstant(ImGuiCol_FrameBgActive);
	AddIMGUIConstant(ImGuiCol_TitleBg);
	AddIMGUIConstant(ImGuiCol_TitleBgActive);
	AddIMGUIConstant(ImGuiCol_TitleBgCollapsed);
	AddIMGUIConstant(ImGuiCol_MenuBarBg);
	AddIMGUIConstant(ImGuiCol_ScrollbarBg);
	AddIMGUIConstant(ImGuiCol_ScrollbarGrab);
	AddIMGUIConstant(ImGuiCol_ScrollbarGrabHovered);
	AddIMGUIConstant(ImGuiCol_ScrollbarGrabActive);
	AddIMGUIConstant(ImGuiCol_CheckMark);
	AddIMGUIConstant(ImGuiCol_SliderGrab);
	AddIMGUIConstant(ImGuiCol_SliderGrabActive);
	AddIMGUIConstant(ImGuiCol_Button);
	AddIMGUIConstant(ImGuiCol_ButtonHovered);
	AddIMGUIConstant(ImGuiCol_ButtonActive);
	AddIMGUIConstant(ImGuiCol_Header); // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
	AddIMGUIConstant(ImGuiCol_HeaderHovered);
	AddIMGUIConstant(ImGuiCol_HeaderActive);
	AddIMGUIConstant(ImGuiCol_Separator);
	AddIMGUIConstant(ImGuiCol_SeparatorHovered);
	AddIMGUIConstant(ImGuiCol_SeparatorActive);
	AddIMGUIConstant(ImGuiCol_ResizeGrip); // Resize grip in lower-right and lower-left corners of windows.
	AddIMGUIConstant(ImGuiCol_ResizeGripHovered);
	AddIMGUIConstant(ImGuiCol_ResizeGripActive);
	AddIMGUIConstant(ImGuiCol_Tab); // TabItem in a TabBar
	AddIMGUIConstant(ImGuiCol_TabHovered);
	AddIMGUIConstant(ImGuiCol_TabActive);
	AddIMGUIConstant(ImGuiCol_TabUnfocused);
	AddIMGUIConstant(ImGuiCol_TabUnfocusedActive);
	AddIMGUIConstant(ImGuiCol_DockingPreview); // Preview overlay color when about to docking something
	AddIMGUIConstant(ImGuiCol_DockingEmptyBg);
	// Background color for empty node (e.g. CentralNode with no window docked into it)
	AddIMGUIConstant(ImGuiCol_PlotLines);
	AddIMGUIConstant(ImGuiCol_PlotLinesHovered);
	AddIMGUIConstant(ImGuiCol_PlotHistogram);
	AddIMGUIConstant(ImGuiCol_PlotHistogramHovered);
	AddIMGUIConstant(ImGuiCol_TableHeaderBg); // Table header background
	AddIMGUIConstant(ImGuiCol_TableBorderStrong); // Table outer and header borders (prefer using Alpha=1.0 here)
	AddIMGUIConstant(ImGuiCol_TableBorderLight); // Table inner borders (prefer using Alpha=1.0 here)
	AddIMGUIConstant(ImGuiCol_TableRowBg); // Table row background (even rows)
	AddIMGUIConstant(ImGuiCol_TableRowBgAlt); // Table row background (odd rows)
	AddIMGUIConstant(ImGuiCol_TextSelectedBg);
	AddIMGUIConstant(ImGuiCol_DragDropTarget); // Rectangle highlighting a drop target
	AddIMGUIConstant(ImGuiCol_NavHighlight); // Gamepad/keyboard: current highlighted item
	AddIMGUIConstant(ImGuiCol_NavWindowingHighlight); // Highlight window when using CTRL+TAB
	AddIMGUIConstant(ImGuiCol_NavWindowingDimBg);
	// Darken/colorize entire screen behind the CTRL+TAB window list, when active
	AddIMGUIConstant(ImGuiCol_ModalWindowDimBg);
	// Darken/colorize entire screen behind a modal window, when one is active
	AddIMGUIConstant(ImGuiCol_COUNT);
	//ICON
	AddIMGUIConstant(ICON_FA_COMPRESS);
	AddIMGUIConstant(ICON_FA_CUBE);
	AddIMGUIConstant(ICON_FA_FOLDER);
	AddIMGUIConstant(ICON_FA_FILE);
	AddIMGUIConstant(ICON_FA_IMAGE);
	AddIMGUIConstant(ICON_FA_LAYER_GROUP);
	AddIMGUIConstant(ICON_FA_MOON);
	AddIMGUIConstant(ICON_FA_CIRCLE);
	AddIMGUIConstant(ICON_FA_CIRCLE_NOTCH);
	AddIMGUIConstant(ICON_FA_ARROWS_ALT);
	AddIMGUIConstant(ICON_FA_IMAGE);
	AddIMGUIConstant(ICON_FA_FILE_IMAGE);
	AddIMGUIConstant(ICON_FA_TREE);
	AddIMGUIConstant(ICON_FA_TABLE_TENNIS);

	imguiModule->Init();
}
}
