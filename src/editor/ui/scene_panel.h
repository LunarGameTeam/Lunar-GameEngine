#pragma once
#include "editor/ui/base_panel.h"

#include "core/event/event_module.h"

#include "engine/camera.h"

#include "window/window.h"
#include "render/render_module.h"





namespace luna::editor
{
class ScenePanel : public PanelBase
{
	RegisterTypeEmbedd(ScenePanel, PanelBase)
public:
	ScenePanel()
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
};
}
