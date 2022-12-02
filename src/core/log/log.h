/*!
 * \file log.h
 * \date 2020/08/09 10:18
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief 日志打印方法
 *
 * TODO: long description
 *
 * \note
*/
#pragma once

#include "core\misc\string.h"
#include <format>
#include "log_msg.h"

#ifndef CORE_API

#ifdef CORE_EXPORT
#define CORE_API __declspec( dllexport )//宏定义
#else
#define CORE_API __declspec( dllimport )
#endif

#endif

namespace luna
{
/*!
 * \class Log
 *
 * \brief Log提供了最基础的格式化日志打印，会输出到控制台以及文件中
 *
 * \author isAk wOng
 *
 */
class CORE_API LogManager
{
public:
	LogManager();
	~LogManager();
};

CORE_API extern bool g_Succeed;
CORE_API extern bool g_Failed;

CORE_API void _LogImpl(
	const char* scope,
	const LString& msg,
	const LogLevel& level,
	const char* file_name,
	const char* function_name,
	const uint32_t line
);

}


/*
 *	建议使用下面6个进行Log
 */
#define LogError(scope, format, ...) luna::_LogImpl(scope ,luna::LString::Format(format, __VA_ARGS__), luna::LogLevel::Error, __FILE__, __FUNCTION__, __LINE__);
#define LogWarning(scope, format,...) luna::_LogImpl(scope,luna::LString::Format(format, __VA_ARGS__), luna::LogLevel::Warning, __FILE__, __FUNCTION__, __LINE__);
#define Log(scope,format,...) luna::_LogImpl(scope, luna::LString::Format(format,__VA_ARGS__), luna::LogLevel::Verbose, __FILE__, __FUNCTION__, __LINE__);
