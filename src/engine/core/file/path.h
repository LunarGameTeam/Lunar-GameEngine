#pragma once

#include "core/misc/string.h"
#include "core/private_core.h"


namespace luna
{

/*!
 * \class LPath
 *
 * \brief 以/开头的为相对路径 非/开头的为绝对路径
 *
 * \author IsakW
 */
class CORE_API LPath
{
public:
	//
	LPath(const char *str);
	LPath(const LString &str);

	LString GetFileNameWithoutExt() const;
	LString GetFileNameWithExt() const;
	LString GetDirectoryName() const;

	const LString &AsString() const
	{
		return m_Path;
	}
	const LString &AsStringAbs() const
	{
		return m_AbsPath;
	}
	bool IsRelative() const;

	static const LString &EngineDir();
	static const LPath &EngineDirPath();

private:
	LString m_AbsPath;
	LString m_Path;


};
}