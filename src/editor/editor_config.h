#pragma once

#include "core/core_config.h"
#include "window/window_config.h"
#include "render/render_config.h"
#include "engine/engine_config.h"

#include "core/core_library.h"

///
/// Render 模块共享全局变量
/// 
namespace luna::editor
{
class EditorModule;

extern EditorModule* sEditorModule;
}
