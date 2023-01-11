#include "editor_module.h"

#include "core/asset/asset_module.h"
#include "core/foundation/profile.h"

#include "game/camera.h"

#include "editor/ui/icon_font.h"

#include "window/window_module.h"
#include "imgui_impl_sdl.h"

#include "render/render_config.h"
#include "render/render_module.h"

#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_cmd_list.h"

#include "imgui_impl_dx12.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"



namespace luna::editor
{
EditorModule* sEditorModule = nullptr;

RegisterTypeEmbedd_Imp(EditorModule)
{
}


EditorModule::EditorModule()
{
	sEditorModule = this;
	mNeedTick = true;
}

bool EditorModule::OnLoad()
{
	return true;
}

bool EditorModule::OnInit()
{
	for (PanelBase* editro : m_editors)
	{
		if (!editro->mInited)
		{
			editro->Init();
			editro->mInited = true;
		}
	}
	return true;
}

bool EditorModule::OnShutdown()
{
	return true;
}

void EditorModule::Tick(float delta_time)
{

}

void EditorModule::OnIMGUI()
{
	for (PanelBase* ed : m_editors)
	{
		ed->DoIMGUI();
	}

}

PanelBase* EditorModule::RegisterPanel(PanelBase* base)
{
	m_editors.push_back(base);

	if (!base->mInited)
	{
		base->Init();
		base->mInited = true;
	}
	return base;
}

MainPanel* EditorModule::GetMainEditor()
{
	if (!mMainEditor)
		mMainEditor = GetEditor < MainPanel>();
	return mMainEditor;
}

}
