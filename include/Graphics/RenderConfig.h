#pragma once

#include "Core/CoreConfig.h"

#include <algorithm>
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
namespace graphics
{
class RenderModule;
class RenderContext;
}


RENDER_API extern graphics::RenderModule* sRenderModule;




}

