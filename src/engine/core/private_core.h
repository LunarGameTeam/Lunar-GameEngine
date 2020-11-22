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

#define __DLL_H__
#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define LunarDLLAPI  __declspec(dllexport)
#else
#define LunarDLLAPI __declspec(dllimport)
#endif

#include "boost/smart_ptr.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/container/string.hpp"
#include "core/memory/ptr.h"
#include "core/misc/container.h"

#include "core/log/log.h"
extern LunarEngine::LogScope E_Core;