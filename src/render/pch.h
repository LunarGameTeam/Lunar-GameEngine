#pragma once

#include "core/pch.h"
#include "window/pch.h"

#include "core/core_library.h"

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>

#include <stdio.h>

///
/// Render 导出宏
/// 
#ifdef RENDER_EXPORT
#define RENDER_API __declspec( dllexport )//宏定义
#else
#define RENDER_API __declspec( dllimport )
#endif


///
/// Render 模块共享全局变量
/// 
namespace luna
{
namespace render
{
class RenderModule;
class RenderDevice;
}


RENDER_API extern render::RenderModule* sRenderModule;




}

