#include "log_msg.h"

#include "windows.h"

//错误信息
LunarEngine::LogMsg::LogMsg()
{
#ifdef  _MSC_VER
	m_Result.m_Result = S_OK;
#endif
	m_MsgContent = "";
	m_LogLevel = LogLevel::Success;
}

LunarEngine::LogMsg::LogMsg(LResult platform_result_in, const std::string& failed_reason_in, const std::string& source, LogLevel logLevel)
{
	m_Result = platform_result_in;
	m_MsgContent = failed_reason_in;
	m_LogLevel = logLevel;
}

void LunarEngine::LogMsg::ShowFailedReason()
{
#ifdef  _MSC_VER
	MessageBox(0, m_MsgContent.c_str(), "error", MB_OK);
#endif
}