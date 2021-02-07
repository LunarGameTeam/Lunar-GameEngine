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

#include "boost/smart_ptr.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/container/string.hpp"
#include "core/memory/ptr.h"
#include "core/misc/container.h"

#include "core/log/log.h"


#ifndef WORLD_API

#ifdef WORLD_EXPORT
#define WORLD_API __declspec( dllexport )//宏定义
#else
#define WORLD_API __declspec( dllimport )
#endif


#endif

extern luna::LogScope E_World;
