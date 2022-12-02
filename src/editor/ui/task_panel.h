#pragma once

#include "editor/ui/base_panel.h"

#include "render/render_module.h"
#include "engine/scene_module.h"


namespace luna::editor
{
class TodoTaskEditor : public EditorBase
{
	RegisterTypeEmbedd(TodoTaskEditor, EditorBase)
public:
	TodoTaskEditor();
	void OnGUI() override;

protected:
	void AddTask(const LString& task, bool done);
};
}
