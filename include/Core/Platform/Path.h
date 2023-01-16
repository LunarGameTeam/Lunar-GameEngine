#pragma once

#include "Core/Foundation/String.h"


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
		return mRelativePath;
	}
	const LString &AsEnginePathString() const
	{
		return mEngineAbsPath;
	}
	const LString& AsProjectPathString() const
	{
		return mProjectAbsPath;
	}
	bool IsRelative() const;

	static const LString &EngineDir();
	static const LPath &EngineDirPath();

	static LString ConvertToEnginePath(const LString& path);
private:
	void Init();
	LString mEngineAbsPath;
	LString mProjectAbsPath;
	LString mRelativePath;
};
}