#include "log.h"
#include "log_msg.h"


#include "core/misc/string.h"

#include <chrono>
#include <iostream>
#include <fstream>

namespace luna
{
std::fstream g_log_file;


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

void _LogImpl(const char *scope, const LString &msg, const LogLevel &level, const char *file_name, const char *function_name, const uint32_t line)
{
	static LogManager s_Log;

	auto start = std::chrono::system_clock::now().time_since_epoch();

	auto const time = std::chrono::current_zone()
		->to_local(std::chrono::system_clock::now());
		
	static const char *level_str[10] =
	{
		"Error",
		"Warning",
		"Success",
		"Verbose",
	};
	LString res = LString::Format("[{:%Y-%m-%d %H:%M:%S}][{}][{}]{}", time, level_str[int(level)], scope, msg);
	
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
	g_log_file << res.c_str()  << std::endl;
	g_log_file.flush();
	SetConsoleTextAttribute(hCon, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
}

}