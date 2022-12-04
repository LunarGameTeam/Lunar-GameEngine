/*!
 * \file window_subsystem.h
 * \date 2020/08/09 11:14
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief
 *
 * TODO: long description
 *
 * \note
*/
#pragma once

#include "core/core_library.h"
#include "core/misc/container.h"
#include "core/config/config.h"

#include "editor/main_editor.h"
#include "editor/ui/task_panel.h"

#include "render/rhi/rhi_descriptor_heap.h"
#include "render/rhi/rhi_types.h"

#include "render/renderer/render_target.h"

#include <d3d12.h>
#include "vulkan/vulkan.h"


namespace luna::editor
{

class HierarchyEditor;
class InspectorEditor;
class SceneEditor;
class LibraryEditor;


class EditorModule : public LModule
{
	RegisterTypeEmbedd(EditorModule, LModule)
public:
	EditorModule();;

public:
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;
	void OnIMGUI() override;

public:
	EditorBase* RegisterEditor(EditorBase* base);


	template<typename T>
	T* GetEditor() 
	{
		for (EditorBase* edi : m_editors)
		{
			if (edi->GetClass()->IsDerivedFrom(LType::Get<T>()))
			{
				return (T*)(edi);
			}
		}
		return nullptr;
	}

	HierarchyEditor* GetHierarchyEditor();
	LibraryEditor* GetLibraryEditor();
	InspectorEditor* GetInspectorEditor();
	SceneEditor* GetSceneEditor();
	MainEditor* GetMainEditor();


private:
	void OnWindowResize(LWindow&, WindowEvent&);

	Scene* mScene;

	render::RHIViewPtr mRtvImgui;
	HierarchyEditor* mHierarchy = nullptr;
	LibraryEditor* mLibrary = nullptr;
	InspectorEditor* mInspector = nullptr;
	TodoTaskEditor* mTask = nullptr;
	SceneEditor* mSceneEditor = nullptr;
	MainEditor* mMainEditor = nullptr;
	
	
	//Render



	render::RHIDevice* mRHIDevice = nullptr;	

	render::RHISwapchainDesc mWindowDesc;
	render::RHIDescriptorPool* mImguiSrvHeap;
	TSubPtr<render::RenderTarget> mScreenRt;
	

	bool mResize = false;
	LVector<EditorBase*> m_editors;

	render::TRHIPtr<render::RHIFence> frame_fence_3D;
};
}
