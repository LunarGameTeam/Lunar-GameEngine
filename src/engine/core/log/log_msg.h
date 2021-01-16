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

/*!
 * \class LogLevel
 *
 * \brief Log级别，Verbose，Warning，Error
 *
 * \author isAk wOng
 *
 */
	enum class LogLevel
	{
		
		Error = 0,
		Warning = 1,
		Success = 2,
		Verbose = 3
	};

/*!
 * \class LResult
 *
 * \brief 兼容Pancy引擎的方法执行返回结果，TODO，LResult在不同平台会有不同的结果（原本是用来取代HRESULT，这个设计并不支持跨平台）
 *
 * \author isAk wOng
 *
 */
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