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


namespace luna
{
	class Log
	{
	public:
		Log();
		~Log();
		std::ofstream m_LogFile;

	};

	extern LResult g_Succeed;
	extern LResult g_Failed;

	struct LogScope
	{
	public:
		LogScope(const char* name)
		{
			strcpy_s(m_ScopeName, name);
		}
		char m_ScopeName[20];
	};

	void LogInternal(
		const LogScope& scope,
		const LString& msg,
		const LogLevel& level,
		const char* file_name,
		const char* function_name,
		const uint32_t line,
		LResult& log_turn = g_Succeed
	);

	void BuildDebugLog(
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
	static char fmt[1000];
	sprintf_s(fmt, format, args...);
	return fmt;
}

#define LogError(scope,result,msg) luna::LogInternal(scope,msg,luna::LogLevel::Error,__FILE__,__FUNCTION__,__LINE__, result);
#define LogWarning(scope,result,msg) luna::LogInternal(scope,msg,luna::LogLevel::Warning,__FILE__,__FUNCTION__,__LINE__, result);
#define LogVerbose(scope,msg) luna::LogInternal(scope, msg ,luna::LogLevel::Verbose,__FILE__,__FUNCTION__,__LINE__, luna::g_Succeed);

#define LogErrorFormat(scope,result,format,...) luna::LogInternal(scope,FormatLog(format,__VA_ARGS__),luna::LogLevel::Error,__FILE__,__FUNCTION__,__LINE__, result);
#define LogWarningFormat(scope,result,format,...) luna::LogInternal(scope,FormatLog(format,__VA_ARGS__),luna::LogLevel::Warning,__FILE__,__FUNCTION__,__LINE__, result);
#define LogVerboseFormat(scope,format,...) luna::LogInternal(scope, FormatLog(format,__VA_ARGS__) ,luna::LogLevel::Verbose,__FILE__,__FUNCTION__,__LINE__, luna::g_Succeed);



#define LunarDebugLogError(platform_result,error_reason,log_index) luna::BuildDebugLog(platform_result,error_reason,__FILE__,__FUNCTION__,__LINE__,luna::LogLevel::Error,log_index)
#define LunarDebugLogWarning(platform_result,error_reason,log_index) luna::BuildDebugLog(platform_result,error_reason,__FILE__,__FUNCTION__,__LINE__,luna::LogLevel::Warning,log_index)
#define LunarDebugLogVerbose(error_reason,log_index) luna::BuildDebugLog(S_OK,error_reason,__FILE__,__FUNCTION__,__LINE__,luna::LogLevel::Verbose,log_index)
