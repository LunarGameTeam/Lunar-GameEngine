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
#include <boost/filesystem.hpp>
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

	extern LResult g_Succeed;
	extern LResult g_Failed;

/*!
 * \class LogScope
 *
 * \brief Log的域，每个模块的私有公共头文件应声明一个该模块的Log域，比如E_Core，E_Render，E_World
 *
 * \author isAk wOng
 *
 */
	struct CORE_API LogScope
	{
	public:
		LogScope(const char* name)
		{
			strcpy_s(m_ScopeName, name);
		}
		char m_ScopeName[20];
	};

	CORE_API void LogInternal(
		const LogScope& scope,
		const LString& msg,
		const LogLevel& level,
		const char* file_name,
		const char* function_name,
		const uint32_t line,
		LResult& log_turn = g_Succeed
	);

	CORE_API void BuildDebugLog(
		const HRESULT& windows_result,
		const LString& error_reason,
		const LString& file_name,
		const LString& function_name,
		const uint32_t& line,
		const LogLevel& log_type,
		LResult& log_turn = g_Succeed
	);


}

template<typename...Args>
char* FormatLog(const char* format, Args&&... args)
{
	//TODO，使用string处理溢出情况
	static char fmt[1000];
	sprintf_s(fmt, format, args...);
	return fmt;
}

/*
 *	建议使用下面6个进行Log
 */
#define LogError(scope,result,msg) luna::LogInternal(scope,msg,luna::LogLevel::Error,__FILE__,__FUNCTION__,__LINE__, result);
#define LogWarning(scope,result,msg) luna::LogInternal(scope,msg,luna::LogLevel::Warning,__FILE__,__FUNCTION__,__LINE__, result);
#define LogVerbose(scope,msg) luna::LogInternal(scope, msg ,luna::LogLevel::Verbose,__FILE__,__FUNCTION__,__LINE__, luna::g_Succeed);
 
#define LogErrorFormat(scope,result,format,...) luna::LogInternal(scope,FormatLog(format,__VA_ARGS__),luna::LogLevel::Error,__FILE__,__FUNCTION__,__LINE__, result);
#define LogWarningFormat(scope,result,format,...) luna::LogInternal(scope,FormatLog(format,__VA_ARGS__),luna::LogLevel::Warning,__FILE__,__FUNCTION__,__LINE__, result);
#define LogVerboseFormat(scope,format,...) luna::LogInternal(scope, FormatLog(format,__VA_ARGS__) ,luna::LogLevel::Verbose,__FILE__,__FUNCTION__,__LINE__, luna::g_Succeed);


/*
 *	弃用的Log函数，目前不支持Format，以及输出到文件
 */
#define LunarDebugLogError(platform_result,error_reason,log_index) luna::BuildDebugLog(platform_result,error_reason,__FILE__,__FUNCTION__,__LINE__,luna::LogLevel::Error,log_index)
#define LunarDebugLogWarning(platform_result,error_reason,log_index) luna::BuildDebugLog(platform_result,error_reason,__FILE__,__FUNCTION__,__LINE__,luna::LogLevel::Warning,log_index)
#define LunarDebugLogVerbose(error_reason,log_index) luna::BuildDebugLog(S_OK,error_reason,__FILE__,__FUNCTION__,__LINE__,luna::LogLevel::Verbose,log_index)
