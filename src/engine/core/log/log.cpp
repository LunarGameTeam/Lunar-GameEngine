
#include "log.h"
#include "log_msg.h"

#include "core/private_core.h"
#include "core/misc/string.h"
#include "boost/format.hpp"
#include "boost/date_time.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>

//log信息(log来源，log错误记录)


namespace luna
{

	LResult g_Succeed;
	LResult g_Failed;

	LString log_file_name;
	std::vector<LogMsg> g_LogQueue;

	void AddLog(const LogMsg& engine_error_message, LResult& error_log);
	void SaveLogToFile(luna::LString log_file_name);
	void PrintLogToconsole();

	Log::Log()
	{
		boost::filesystem::directory_entry dirPath("log");
		boost::filesystem::path logPath("log\\log.txt");
		if (!boost::filesystem::exists(dirPath))
			boost::filesystem::create_directory(dirPath);
		m_LogFile.open(logPath.c_str());
	}

	Log::~Log()
	{
		m_LogFile.flush();
		m_LogFile.close();
	}

}

void luna::AddLog(const luna::LogMsg& engine_error_message, luna::LResult& error_log)
{
	g_LogQueue.push_back(engine_error_message);
}

void luna::LogInternal(const LogScope& scope, const LString& msg, const LogLevel& level, const char* file_name, const char* function_name, const uint32_t line, LResult& log_turn /*= succeed */)
{
	static Log s_Log;
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
	sprintf_s(fmt, "[%04lld.%02lld.%02lld-%02lld.%02lld.%02lld:%03lld][Verbose][%s]%s", y, month, d, h, m, s, ms, scope.m_ScopeName, msg.c_str());
	std::cout << fmt << std::endl;
	s_Log.m_LogFile << fmt << std::endl;
	s_Log.m_LogFile.flush();
}

void luna::BuildDebugLog(
	const HRESULT& windows_result,
	const luna::LString& error_reason,
	const luna::LString& file_name,
	const luna::LString& function_name,
	const uint32_t& line,
	const LogLevel& log_type,
	luna::LResult& log_turn
)
{
	std::string new_log_source = "file: " + file_name.str() + " \nfunction: " + function_name.str() + " \nline " + std::to_string(line) + "\n" + error_reason.c_str();
	LResult result;
	result.m_Result = windows_result;
	if (log_type == LogLevel::Error) 
	{
		result.m_IsOK = false;
		MessageBoxA(0, new_log_source.c_str(),"error",MB_OK);
	}
	LogMsg new_message(result, error_reason.str(), new_log_source, log_type);
	AddLog(new_message, log_turn);
}
