#include "editor/ui/inspector_panel.h"

#include "core/object/component.h"
#include "core/object/entity.h"
#include "core/object/transform.h"
#include "core/event/event_module.h"
#include "core/asset/asset_module.h"
#include "window/window.h"

#include "engine/scene_module.h"
#include "engine/mesh_renderer.h"
#include "engine/camera.h"

#include "editor/ui/property_helper.h"
#include "editor/ui/icon_font.h"


namespace luna::editor
{

RegisterTypeEmbedd_Imp(InspectorEditor)
{
	cls->Ctor<InspectorEditor>();
	cls->Binding<InspectorEditor>();
	BindingModule::Get("luna.editor")->AddType(cls);
};


void InspectorEditor::OnGUI()
{
	if (m_cur_entity)
	{
		GUIComponentEditor();
		// 			if (m_cur_entity)
		// 			{
		// 				ImGui::TableNextRow();
		// 				ImGui::TableSetColumnIndex(0);
		// 				ImGui::Text("Name");
		// 				ImGui::TableSetColumnIndex(1);
		// 				LString name = m_cur_entity->GetObjectName();
		// 				if (InputLString("##Name", &name, ImGuiInputTextFlags_CallbackAlways))
		// 				{
		// 					m_cur_entity->SetObjectName(name);
		// 				}
		// 			}
	}
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("MeshRenderer"))
			{
				m_cur_entity->AddComponent<render::MeshRenderer>();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void InspectorEditor::GUIComponentEditor()
{
	for (TSubPtr<Component>& comp_it : m_cur_entity->GetAllComponents())
	{
		Component* comp = comp_it.Get();
		GUIComponentProperty(comp);
	}
}

void InspectorEditor::GUIComponentProperty(Component* comp)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImGui::PushID(comp);
	LType* comp_type = comp->GetClass();
	LString comp_name = comp_type->GetName();
	std::vector<LProperty*> properties;
	comp_type->GetAllProperties(properties);
	int i = 0;
	bool node_open = ImGui::TreeNodeEx(comp, ImGuiTreeNodeFlags_DefaultOpen, "%s %s", ICON_FA_COMPRESS,
	                                   comp_type->GetName().c_str());

	float start_x = 0;
	float start_y = ImGui::GetCursorScreenPos().y;

	for (LProperty* prop : properties)
	{
		float porp_width = ImGui::CalcTextSize(prop->GetNameStr()).x + 30;
		m_prop_name_width = std::min<float>(250.0f, std::max<float>(porp_width, m_prop_name_width));
	}

	if (node_open)
	{
		for (LProperty* prop : properties)
		{
			ImGui::PushID(i);
			if (prop->GetName() != "name")
			{
				ImGui::Text(prop->GetNameStr());
				ImGui::SameLine(m_prop_name_width, 16);
				start_x = ImGui::GetCursorScreenPos().x - 8;
				property_helper_func(prop, comp);
			}
			ImGui::PopID();
			i++;
		}
		float end_y = ImGui::GetCursorScreenPos().y;
		draw_list->AddLine(ImVec2(start_x, start_y), ImVec2(start_x, end_y), IM_COL32(50, 50, 50, 255), 1.0f);
		ImGui::TreePop();
	}
	ImGui::PopID();
}
}
