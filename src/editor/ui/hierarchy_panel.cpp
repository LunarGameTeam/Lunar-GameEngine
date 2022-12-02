#include "editor/ui/hierarchy_panel.h"
#include "editor/ui/inspector_panel.h"
#include "core/object/transform.h"
#include "editor/editor_module.h"
#include "editor/ui/icon_font.h"
#include <imgui_internal.h>


namespace luna::editor
{

RegisterTypeEmbedd_Imp(HierarchyEditor)
{
	cls->Ctor<HierarchyEditor>();
	cls->Binding<HierarchyEditor>();
	LBindingModule::Get("luna.editor")->AddType(cls);
};

void HierarchyEditor::OnGUI()
{
	Scene* main_scene = sSceneModule->GetScene(0);
	if (!main_scene)
		return;
	auto& enties = main_scene->GetAllEntities();
	if (ImGui::BeginPopupContextVoid("HierarchyMenu", ImGuiMouseButton_Right))
	{
		if (ImGui::BeginMenu("创建"))
		{
			if (ImGui::MenuItem("Entity"))
			{
				ActionCreateEntity(nullptr);
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	for (int i = 0; i < enties.Size(); i++)
	{
		Entity* it = enties[i];
		ImGui::PushID(it);
		int flags = 0;
		if (it->GetChildCount() == 0)
			flags = flags | ImGuiTreeNodeFlags_Leaf;
		if (m_select_entity == it)
			flags = flags | ImGuiTreeNodeFlags_Selected;
		if (it->GetParent() == nullptr)
			flags = flags | ImGuiTreeNodeFlags_DefaultOpen;
		if (CustomTreeNode((const char*)it, flags, [&](bool hoverd, bool held)
		{
			LString name = LString::Format("{0} {1}", ICON_FA_CUBE, it->GetObjectName());
			if (held)
			{
				m_select_entity = it;
				sEditorModule->GetInspectorEditor()->SetCurrentEntity(it);
			}
			ImGui::Text(name.c_str());
		}))
		{
			ImGui::TreePop();
		}

		if (ImGui::BeginPopupContextItem("Hierarchy Item Menu", ImGuiMouseButton_Right))
		{
			if (ImGui::BeginMenu("创建"))
			{
				if (ImGui::MenuItem("Entity"))
				{
					ActionCreateEntity(it);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Add Component"))
			{
				ImGui::MenuItem("MeshRenderer");
				ImGui::EndMenu();
			}
			ImGui::Separator();
			ImGui::MenuItem("Destroy");
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Entity"))
			{
				auto* entity = sSceneModule->GetScene(0)->CreateEntity("Entity1");
				entity->RequireComponent<Transform>();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

void HierarchyEditor::ActionCreateEntity(Entity* parent)
{
	Scene* main_scene = sSceneModule->GetScene(0);
	Entity* entity = main_scene->CreateEntity("Entity", parent);
	entity->RequireComponent<Transform>();
}
}
