#include "editor_module.h"

#include "core/asset/asset_module.h"
#include "core/foundation/profile.h"

#include "engine/camera.h"

#include "editor/ui/hierarchy_panel.h"
#include "editor/ui/scene_panel.h"
#include "editor/ui/icon_font.h"
#include "editor/ui/inspector_panel.h"
#include "editor/ui/library_panel.h"

#include "window/window_module.h"
#include "imgui_impl_sdl.h"

#include "render/render_config.h"
#include "render/render_module.h"

#include "render/rhi/rhi_frame_buffer.h"
#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_device.h"
#include "render/rhi/rhi_cmd_list.h"

#include "imgui_impl_dx12.h"
#include "render/rhi/DirectX12/dx12_command_list.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/rhi/DirectX12/dx12_descriptor_pool.h"
#include "render/rhi/DirectX12/dx12_view.h"

#include "imgui_impl_dx12.h"
#include "imgui_impl_vulkan.h"
#include "render/rhi/vulkan/vulkan_device.h"
#include "render/rhi/vulkan/vulkan_render_queue.h"
#include "render/rhi/vulkan/vulkan_descriptor_pool.h"
#include "render/rhi/vulkan/vulkan_render_pass.h"
#include "render/rhi/vulkan/vulkan_cmd_list.h"
#include "render/rhi/vulkan/vulkan_resource.h"
#include "render/rhi/vulkan/vulkan_view.h"




namespace luna::editor
{
EditorModule* sEditorModule = nullptr;

static void ImGUIText(const LString& name)
{
	ImGui::Text(name.c_str());
}

RegisterTypeEmbedd_Imp(EditorModule)
{
	cls->BindingMethod<&EditorModule::RegisterEditor>("register_editor").GetBindingMethodDef().ml_doc = LString::MakeStatic("def register_editor(self, t: typing.Type[T] ) -> T:\n\tpass\n");

	cls->Binding<EditorModule>();

	BindingModule* editor_module = BindingModule::Get("luna.editor");
	editor_module->AddType(cls);
	BindingModule::Get("luna.editor.IMGUIFlags")->AddConstant("TextClip", 1);
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
	auto func = std::bind(&EditorModule::OnWindowResize, this, std::placeholders::_1, std::placeholders::_2);
	static auto handle = sWindowModule->OnWindowResize.Bind(func);
		
	gEngine->GetModule<render::RenderModule>();

	for (EditorBase* editro : m_editors)
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


	render::RenderDevice* device = sRenderModule->GetRenderDevice();
}

void EditorModule::OnIMGUI()
{
	for (EditorBase* ed : m_editors)
	{
		ed->DoIMGUI();
	}
// 	if (ImGui::TreeNode("Editor"))
// 	{
// 		ImGui::Text("PyLObject数量: %d", binding:: BindingLObject::sBindingObjectNum);
// 		ImGui::TreePop();
// 	}

}

void EditorModule::OnWindowResize(LWindow&, WindowEvent& evt)
{
	GetMainEditor()->mWidth = evt.width;
	GetMainEditor()->mHeight = evt.height;

	mWindowDesc.mFrameNumber = 2;
	mWindowDesc.mHeight = evt.height;
	mWindowDesc.mWidth = evt.width;
	mNeedResize = true;

}

EditorBase* EditorModule::RegisterEditor(EditorBase* base)
{
	m_editors.push_back(base);

	if (!base->mInited)
	{
		base->Init();
		base->mInited = true;
	}
	return base;
}

InspectorEditor* EditorModule::GetInspectorEditor()
{
	if (!mInspector)
		mInspector = GetEditor < InspectorEditor>();
	return mInspector;
}

SceneEditor* EditorModule::GetSceneEditor()
{
	if (!mSceneEditor)
		mSceneEditor = GetEditor<SceneEditor>();
	return mSceneEditor;
}

HierarchyEditor* EditorModule::GetHierarchyEditor()
{

	if (!mHierarchy)
		mHierarchy = GetEditor < HierarchyEditor>();
	return mHierarchy;
}

LibraryEditor* EditorModule::GetLibraryEditor()
{
	if (!mLibrary)
		mLibrary = GetEditor < LibraryEditor>();
	return mLibrary;
}

MainEditor* EditorModule::GetMainEditor()
{
	if (!mMainEditor)
		mMainEditor = GetEditor < MainEditor>();
	return mMainEditor;
}

void EditorModule::OnFrameEnd(float deltaTime)
{

	if (mNeedResize)
	{
		sRenderModule->GetSwapChain()->Reset(mWindowDesc);
		sRenderModule->UpdateFrameBuffer();
		mNeedResize = false;
	}
}

}
