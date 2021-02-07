#include "editor_subsystem.h"

#include "window/window_subsystem.h"

namespace luna
{
	namespace editor
	{

		bool EditorSubsystem::OnPreInit()
		{
			return true;
		}

		bool EditorSubsystem::OnPostInit()
		{
			return true;
		}

		bool EditorSubsystem::OnInit()
		{
			m_editors.push_back(new MainEditor());
			m_editors.push_back(new SceneEditor());
			m_editors.push_back(new LibraryEditor());
			m_editors.push_back(new HierarchyEditor());
			m_editors.push_back(new InspectorEditor());
			return true;
		}

		bool EditorSubsystem::OnShutdown()
		{
			return true;
		}

		void EditorSubsystem::Tick(float delta_time)
		{
			

		}

		void EditorSubsystem::OnIMGUI()
		{
			for (auto& editor : m_editors)
			{
				editor->OnGUI();
			}		
		}

	}
}

