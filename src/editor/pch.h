#pragma once

#include "core/pch.h"
#include "window/pch.h"
#include "render/pch.h"
#include "engine/pch.h"

#include "core/core_library.h"

///
/// Render 模块共享全局变量
/// 
namespace luna::editor
{
class EditorModule;

extern EditorModule* sEditorModule;
}
