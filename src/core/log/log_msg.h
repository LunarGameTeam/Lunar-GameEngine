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
enum class CORE_API LogLevel
{
	Error = 0,
	Warning = 1,
	Success = 2,
	Verbose = 3
};

}