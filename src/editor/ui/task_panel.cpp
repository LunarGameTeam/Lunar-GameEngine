#include "editor/ui/task_panel.h"
#include "editor/ui/icon_font.h"


namespace luna::editor
{


RegisterTypeEmbedd_Imp(TodoTaskEditor)
{
	cls->Ctor<TodoTaskEditor>();
	cls->Binding<TodoTaskEditor>();
};

TodoTaskEditor::TodoTaskEditor()
{
	mTitle = "ToDoTask";
}

void TodoTaskEditor::OnGUI()
{	
	AddTask("临时Descriptor Allocator", true);
	AddTask("FrameGraph", false);
}

void TodoTaskEditor::AddTask(const LString& task, bool done)
{
	if (done)
		ImGui::Text(ICON_FA_CHECK);
	else
		ImGui::Text(ICON_FA_CIRCLE);

	ImGui::SameLine(30);
	ImGui::Text(task.c_str());
}

}