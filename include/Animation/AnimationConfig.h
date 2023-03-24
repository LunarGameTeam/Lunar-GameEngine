#pragma once

#include "Core/CoreConfig.h"

#include <algorithm>
#include <memory>
#include <stdexcept>

#include <stdio.h>

///
/// ANIMATION_EXPORT 导出宏
/// 
#ifdef ANIMATION_EXPORT
#define ANIMATION_API __declspec( dllexport )//宏定义
#else
#define ANIMATION_API __declspec( dllimport )
#endif


///
/// ANIMATION 模块共享全局变量
/// 
namespace luna
{
namespace animation
{
class AnimationModule;
}


ANIMATION_API extern animation::AnimationModule* sAnimationModule;




}

