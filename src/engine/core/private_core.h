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

#include <wtypes.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
//STL容器
#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <queue>
//boost
#include <boost/array.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/container/string.hpp>
//c++运行时
#include <string>
#include <fstream>
#include <typeinfo>
#include <atomic>
//内存泄漏检测
#define CheckWindowMemory
#ifdef CheckWindowMemory
#include <crtdbg.h>
 //#ifdef _DEBUG
 //#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)  
 //#endif
#endif
#define LunarResourceID uint16_t
#define LunarObjectID uint32_t
#define LunarResourceSize uint64_t
#ifdef  _MSC_VER
#include<windows.h>
#define LunarHResult HRESULT
#endif

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