
#include "log.h"
#include "log_msg.h"

#include "core/private_core.h"
#include "core/misc/string.h"
#include "boost/format.hpp"
#include "boost/date_time.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>


namespace luna
{
	std::fstream g_log_file;

	LResult g_Succeed;
	LResult g_Failed;

	std::vector<LogMsg> g_LogQueue;

	LogManager::LogManager()
	{
#ifdef WIN32
		TCHAR tempPath[1000];
		GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录

		LString config_path(tempPath);
		config_path = config_path + "/log.txt";		

#endif // WIN32
		g_log_file.open(config_path.c_str(), std::fstream::out);
	}

	LogManager::~LogManager()
	{
		g_log_file.flush();
		g_log_file.close();
	}


	void LogInternal(const LogScope &scope, const LString &msg, const LogLevel &level, const char *file_name, const char *function_name, const uint32_t line, LResult &log_turn /*= succeed */)
	{
		static LogManager s_Log;
		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		const boost::posix_time::time_duration td = now.time_of_day();
		auto dt = now.date();
		__int64 y = dt.year();
		__int64 month = dt.month();
		__int64 d = dt.day();
		__int64 h = td.hours();
		__int64 m = td.minutes();
		__int64 s = td.seconds();
		__int64 ms = td.total_milliseconds() - ((h * 3600 + m * 60 + s) * 1000);
		//据说format有性能问题，此处使用sprintf
		/*
		boost::format fmt;
		switch (level)
		{
		case Verbose:
			fmt = boost::format("[%04d.%02d.%02d-%02d.%02d.%02d:%03d][Verbose][%s]%s") % y % month % d % h % m % s % ms % scope.m_ScopeName % msg.str();
			break;
		case Warning:
			fmt = boost::format("[%04d.%02d.%02d-%02d.%02d.%02d:%03d][Warning][%s]%s") % y % month % d % h % m % s % ms % scope.m_ScopeName % msg.str();
			break;
		case Error:
			fmt = boost::format("[%04d.%02d.%02d-%02d.%02d.%02d:%03d][Error  ][%s]%s\nfile:%s\nfunction:%s\nline:%d") % y % month % d % h % m % s % ms % scope.m_ScopeName % msg.str() % file_name%function_name%line;
			break;
		default:
			break;
		}
		*/
		static char fmt[1000];
		static const char *level_str[10] =
		{
			"Error",
			"Warning",
			"Success",
			"Verbose",
		};
		sprintf_s(fmt, "[%04lld.%02lld.%02lld-%02lld.%02lld.%02lld:%03lld][%s][%s]%s", y, month, d, h, m, s, ms, level_str[int(level)], scope.m_ScopeName, msg.c_str());
		std::cout << fmt << std::endl;
		g_log_file << fmt << std::endl;
		g_log_file.flush();
	}

	void BuildDebugLog(
		const HRESULT &windows_result,
		const LString &error_reason,
		const LString &file_name,
		const LString &function_name,
		const uint32_t &line,
		const LogLevel &log_type,
		LResult &log_turn
	)
	{
		std::string new_log_source = "file: " + file_name.str() + " \nfunction: " + function_name.str() + " \nline " + std::to_string(line) + "\n" + error_reason.c_str();
		LResult result;
		result.m_Result = windows_result;
		if (log_type == LogLevel::Error)
		{
			result.m_IsOK = false;
			MessageBoxA(0, new_log_source.c_str(), "error", MB_OK);
		}
		LogMsg new_message(result, error_reason.str(), new_log_source, log_type);
	}

}
