#pragma once

#include "editor/ui/base_panel.h"
#include "core/object/entity.h"
#include "core/event/EventModule.h"

#include "window/window.h"

#include "engine/scene_module.h"


namespace luna::editor
{
class InspectorEditor : public EditorBase
{
	RegisterTypeEmbedd(InspectorEditor, EditorBase)
public:
	InspectorEditor()
	{
		mTitle = "Inspector";
	}

	void OnGUI() override;

	void SetCurrentEntity(Entity* entity)
	{
		m_cur_entity = entity;
	}

	Entity* m_cur_entity = nullptr;
private:
	void GUIComponentEditor();
	void GUIComponentProperty(Component* comp);
	void GUIProperty(LProperty* prop, LObject* obj);
	float m_prop_name_width = 100;
};
}
