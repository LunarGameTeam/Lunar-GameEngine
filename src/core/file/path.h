#pragma once

#include "core/misc/string.h"


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
	LPath() {};
	LPath(const char *str);
	LPath(const LString &str);

	LString GetFileNameWithoutExt() const;
	LString GetFileNameWithExt() const;
	LString GetDirectoryName() const;

	const LString &AsString() const
	{
		return mPath;
	}
	const LString &AsStringAbs() const
	{
		return mAbsPath;
	}
	bool IsRelative() const;

	static const LString &EngineDir();
	static const LPath &EngineDirPath();

	static LString ConvertToEnginePath(const LString& path);
private:
	LString mAbsPath;
	LString mPath;
};
}