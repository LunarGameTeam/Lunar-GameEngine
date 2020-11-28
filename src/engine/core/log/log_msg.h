#pragma once
/*!
 * \file log_msg.h
 * \date 2020/08/11 14:03
 *
 * \author isakwang
 * Contact: user@company.com
 *
 * \brief 构造LogMessage
 *
 * TODO: long description
 *
 * \note
*/

#include "core\misc\string.h"

#include "windows.h"

namespace luna
{

	enum class LogLevel
	{
		Success = 0,
		Warning,
		Error,
		Verbose
	};

	struct LResult
	{
#ifdef _MSC_VER
		HRESULT m_Result;
#endif
		bool m_IsOK = true;
	};
	//错误返回信息
	class LogMsg
	{
	public:
		LogMsg();
		LogMsg(LResult platform_result_in, const std::string& failed_reason_in, const std::string& source, LogLevel log_type_in = LogLevel::Error);

		LResult m_Result;
		LString m_SourceInfo;
		LString m_MsgContent;
		LogLevel m_LogLevel;
	private:
		void ShowFailedReason();
	};

}