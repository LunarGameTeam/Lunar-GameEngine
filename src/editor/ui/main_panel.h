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

#include "editor/editor_config.h"
#include "editor/ui/base_panel.h"
#include "core/core_library.h"
#include "core/foundation/container.h"
#include "window/window_module.h"


namespace luna::editor
{
class MainPanel : public PanelBase
{
	RegisterTypeEmbedd(MainPanel, PanelBase);
public:
	MainPanel() = default;
	void OnInputEvent(LWindow& window, InputEvent& event);
	void Init() override;
	void OnWindowResize(LWindow& window, WindowEvent& evt);
	void DoIMGUI() override;

	uint32_t mWidth = 1024;
	uint32_t mHeight = 768;
private:
	bool open = true;
	int moveOffsetX = 0;
	int moveOffsetY = 0;
};
}
