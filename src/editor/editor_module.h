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
#include "core/foundation/container.h"
#include "core/foundation/config.h"

#include "editor/ui/main_panel.h"

#include "render/rhi/rhi_descriptor_heap.h"
#include "render/rhi/rhi_types.h"

#include "render/renderer/render_target.h"

#include <d3d12.h>
#include "vulkan/vulkan.h"
#include "game/scene.h"


namespace luna::editor
{

class HierarchyEditor;
class InspectorEditor;
class ScenePanel;
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
	void OnFrameEnd(float deltaTime) override;

public:
	PanelBase* RegisterPanel(PanelBase* base);


	template<typename T>
	T* GetEditor() 
	{
		for (PanelBase* edi : m_editors)
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
	ScenePanel* GetSceneEditor();
	MainPanel* GetMainEditor();


private:
	void OnWindowResize(LWindow&, WindowEvent&);

	Scene* mScene;

	render::RHIViewPtr mRtvImgui;
	ScenePanel* mSceneEditor = nullptr;
	MainPanel* mMainEditor = nullptr;
	
	
	//Render



	render::RHIDevice* mRHIDevice = nullptr;	

	render::RHISwapchainDesc mWindowDesc;
	render::RHIDescriptorPool* mImguiSrvHeap;
	render::RenderTarget* mScreenRt;
	

	bool mNeedResize = false;
	LArray<PanelBase*> m_editors;

	render::TRHIPtr<render::RHIFence> frame_fence_3D;
};
}
