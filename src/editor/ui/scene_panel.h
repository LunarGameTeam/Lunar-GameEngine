#pragma once
#include "editor/ui/base_panel.h"

#include "core/event/EventModule.h"

#include "engine/camera.h"

#include "window/window.h"
#include "render/render_module.h"





namespace luna::editor
{
class SceneEditor : public EditorBase
{
	RegisterTypeEmbedd(SceneEditor, EditorBase)
public:
	SceneEditor()
	{
		mTitle = "Scene";
	}

	void OnGUI() override;
	void Init() override;
	void OnInputEvent(LWindow& window, InputEvent& event);
	void UpdateRT();
	void SetScene(Scene* val);

protected:
	Scene*                mScene      = nullptr;
	CameraComponent*      mCamera     = nullptr;

	bool                  mNeedUpdate = false;
	bool                  mDragging   = false;
	ImVec2                mOldSize    = ImVec2(0, 0);
	render::ImguiTexture* mImguiTexture = nullptr;
};
}
