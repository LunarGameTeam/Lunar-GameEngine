#include "core/file/path.h"
#include "core/file/platform_module.h"
#include "core/file/platform_file.h"
#include "core/framework/luna_core.h"

#include <filesystem>

using namespace luna;

luna::LString LPath::GetDirectoryName() const
{
	return "";
}

luna::LString LPath::GetFileNameWithoutExt() const
{
	return "";
}

LPath::LPath(const char *str) : mPath(str)
{
	static PlatformModule *fileSub = gEngine->GetModule<PlatformModule>();
	assert(fileSub != nullptr);
	static IPlatformFileManager *file = fileSub->GetPlatformFileManager();
	assert(file != nullptr);

	mPath.ReplaceAll("\\", "/");
	//Relative Path
	if (mPath.StartWith('/'))
	{
		mAbsPath = file->EngineDir() + str;
	}
	else
	{
		mAbsPath = str;
	}
}

LPath::LPath(const LString &str) :mPath(str)
{
	static PlatformModule *fileSub = gEngine->GetModule<PlatformModule>();
	assert(fileSub != nullptr);
	static IPlatformFileManager *file = fileSub->GetPlatformFileManager();
	assert(file != nullptr);

	mPath.ReplaceAll("\\", "/");
	//Relative Path
	if (mPath.StartWith('/'))
	{
		mAbsPath = file->EngineDir() + str;
	}
	else
	{
		mAbsPath = str;
	}
}

LString luna::LPath::GetFileNameWithExt() const
{
	return "";
}

bool LPath::IsRelative() const
{
	return mPath.StartWith("/");
}

luna::LString LPath::ConvertToEnginePath(const LString& path)
{
	static IPlatformFileManager* file = sPlatformModule->GetPlatformFileManager();
	
	std::filesystem::path full_path(path.c_str());
	static std::filesystem::path engine_path(file->EngineDir().c_str());
	auto relative_path = std::filesystem::relative(full_path, engine_path);
	return LString(relative_path.string());
}
