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


#ifndef GAME_API

#ifdef GAME_EXPORT
#define GAME_API __declspec( dllexport )//宏定义
#else
#define GAME_API __declspec( dllimport )
#endif


#endif

///
/// Render 模块共享全局变量
/// 
namespace luna
{
class GameModule;

GAME_API extern GameModule *sGameModule;
}

