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

#include "core/core_config.h"

#include "core/memory/ptr.h"
#include "core/foundation/container.h"
#include "core/foundation/log.h"
#include "core/math/math.h"


#ifndef WINDOW_API
#ifdef WINDOW_EXPORT
#define WINDOW_API __declspec( dllexport )//宏定义
#else
#define WINDOW_API __declspec( dllimport )
#endif
#endif

