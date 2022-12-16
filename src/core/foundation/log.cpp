#include "core/foundation/log.h"
#include "core/foundation/string.h"

#include <windows.h>
#include <chrono>
#include <iostream>
#include <fstream>

namespace luna
{

void Logger::RedirectLogFile(const std::string& filePath)
{
	mLogFile.open(filePath.c_str(), std::ios::out);
}

void _LogImpl(const char *scope, const LString &msg, const LogLevel &level, const char *file_name, const char *function_name, const uint32_t line)
{
	static Logger& s_Log = Logger::instance();

	auto start = std::chrono::system_clock::now().time_since_epoch();

	auto const time = std::chrono::current_zone()
		->to_local(std::chrono::system_clock::now());
		
	static const char *logLevel[10] =
	{
		"Error",
		"Warning",
		"Success",
		"Verbose",
	};
	LString res = LString::Format("[{:%Y-%m-%d %H:%M:%S}][{}][{}]{}", time, logLevel[int(level)], scope, msg);
	
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (level)
	{
	default:
		break;
	case LogLevel::Error:
		SetConsoleTextAttribute(hCon, FOREGROUND_INTENSITY | FOREGROUND_RED);
		break;
	case LogLevel::Warning:
		SetConsoleTextAttribute(hCon, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case LogLevel::Success:
	case LogLevel::Verbose:
		SetConsoleTextAttribute(hCon, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	}
	std::cout << res.c_str() << std::endl;
	if (s_Log.mLogFile.is_open())
	{
		s_Log.mLogFile << res.c_str() << std::endl;
		s_Log.mLogFile.flush();
	}	
	SetConsoleTextAttribute(hCon, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
}

}