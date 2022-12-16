/*!
 * \file private_core.h
 * \date 2020/08/09 10:05
 *
 * \author IsakWong 
 *
 * \brief Core模块内部的预处理头文件，处理一些Core模块公共的内部定义
 *
 * 
 *
 * \note
*/
#pragma once

#include "core/memory/ptr.h"
#include "core/foundation/container.h"
#include "core/foundation/log.h"
#include "core/core_config.h"


#ifndef ENGINE_API

#ifdef ENGINE_EXPORT
#define ENGINE_API __declspec( dllexport )//宏定义
#else
#define ENGINE_API __declspec( dllimport )
#endif


#endif

///
/// Render 模块共享全局变量
/// 
namespace luna
{
class SceneModule;

ENGINE_API extern SceneModule *sSceneModule;
}

