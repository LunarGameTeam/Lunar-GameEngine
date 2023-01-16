#include "Core/Foundation/Log.h"
#include "Core/Foundation/String.h"

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
	static Logger& sLog = Logger::instance();

	auto start = std::chrono::system_clock::now().time_since_epoch();

	auto const time = std::chrono::current_zone()
		->to_local(std::chrono::system_clock::now());
		
	static const char *logLevel[10] =
	{
		"E",
		"W",
		"V",
	};
	using namespace std::chrono;

	auto t = time.time_since_epoch();
	auto m = duration_cast<minutes>(t);
	t -= m;
	auto s = duration_cast<seconds>(t);
	t -= s;
	auto mm = duration_cast<milliseconds>(t);	

	static LString res;

	res = LString::Format("[{:%H:%M}:{:02}.{:03} {}-{:6}]{}", time, s.count(),  mm.count(), logLevel[int(level)], scope, msg);

	if (sLog.mLogFile.is_open())
	{
		sLog.mLogFile << res.c_str() << std::endl;
		sLog.mLogFile.flush();
	}

#ifdef WIN32
	static HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
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
	case LogLevel::Verbose:
		SetConsoleTextAttribute(hCon, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	}
	std::cout << res.c_str() << std::endl;
	SetConsoleTextAttribute(hCon, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
#endif // WIN32

}

}