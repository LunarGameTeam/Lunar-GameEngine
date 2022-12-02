#pragma once

#include "editor/ui/base_panel.h"

#include "render/render_module.h"
#include "engine/scene_module.h"


namespace luna::editor
{
class HierarchyEditor : public EditorBase
{
	RegisterTypeEmbedd(HierarchyEditor, EditorBase);
public:
	HierarchyEditor()
	{
		mTitle = "Hierarchy";
	}

	void OnGUI() override;

protected:
	Entity* m_select_entity = nullptr;
	void ActionCreateEntity(Entity* parent);
};
}
