#include "Core/Platform/Path.h"
#include "Core/Platform/PlatformModule.h"
#include "Core/Platform/PlatformFile.h"
#include "Core/Framework/LunaCore.h"

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

LPath::LPath(const char *str) : mRelativePath(str)
{
	Init();
}

LPath::LPath(const LString &str) : mRelativePath(str)
{
	Init();
}

void LPath::Init()
{
	static PlatformModule* fileSub = gEngine->GetModule<PlatformModule>();
	assert(fileSub != nullptr);
	static IPlatformFileManager* file = fileSub->GetPlatformFileManager();
	assert(file != nullptr);
	if (mRelativePath.Find(":/") == std::string::npos)
	{
		mRelativePath.ReplaceAll("\\", "/");
		if (!mRelativePath.StartWith('/'))
			mRelativePath = "/" + mRelativePath;
		//Relative Path
		mEngineAbsPath = file->EngineDir() + mRelativePath;
		mProjectAbsPath = file->ProjectDir() + mRelativePath;
	}
	else
	{
		mEngineAbsPath = mRelativePath;
		mProjectAbsPath = mRelativePath;
	}
}


LString luna::LPath::GetFileNameWithExt() const
{
	return "";
}

bool LPath::IsRelative() const
{
	return mRelativePath.StartWith("/");
}

luna::LString LPath::ConvertToEnginePath(const LString& path)
{
	static IPlatformFileManager* file = sPlatformModule->GetPlatformFileManager();
	
	std::filesystem::path full_path(path.c_str());
	static std::filesystem::path engine_path(file->EngineDir().c_str());
	auto relative_path = std::filesystem::relative(full_path, engine_path);
	return LString(relative_path.string());
}
