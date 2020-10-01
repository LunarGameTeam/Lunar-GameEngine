#include "path.h"
#include "file_subsystem.h"
#include "platform_file.h"
#include "core\engine\engine.h"

using namespace LunarEngine;

LunarEngine::LString LPath::GetDirectoryName() const
{
	return "";
}

LunarEngine::LString LPath::GetFileNameWithoutExt() const
{
	return "";
}

LPath::LPath(const char* str) : m_Path(str)
{
	static FileSubsystem* fileSub = gEngine->GetSubsystem<FileSubsystem>().get();
	assert(fileSub != nullptr);
	static IPlatformFileManager* file = fileSub->GetPlatformFileManager();
	assert(file != nullptr);

	m_Path.ReplaceAll("\\", "/");
	//Relative Path
	if (m_Path.StartWith('/'))
	{
		m_AbsPath = file->EngineDir() + str;
	}
}

LPath::LPath(const LString& str) :m_Path(str)
{
	static FileSubsystem* fileSub = gEngine->GetSubsystem<FileSubsystem>().get();
	assert(fileSub != nullptr);
	static IPlatformFileManager* file = fileSub->GetPlatformFileManager();
	assert(file != nullptr);

	m_Path.ReplaceAll("\\", "/");
	//Relative Path
	if (m_Path.StartWith('/'))
	{
		m_AbsPath = file->EngineDir() + str;
	}
}

LunarEngine::LString LunarEngine::LPath::GetFileNameWithExt() const
{
	return "";
}

bool LunarEngine::LPath::IsRelative() const
{
	return m_Path.StartWith("/");
}

