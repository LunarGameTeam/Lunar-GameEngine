#include "path.h"
#include "file_subsystem.h"
#include "platform_file.h"

using namespace luna;

luna::LString LPath::GetDirectoryName() const
{
	return "";
}

luna::LString LPath::GetFileNameWithoutExt() const
{
	return "";
}

LPath::LPath(const char* str) : m_Path(str)
{
	static FileSubsystem* fileSub = gEngine->GetSubsystem<FileSubsystem>();
	assert(fileSub != nullptr);
	static IPlatformFileManager* file = fileSub->GetPlatformFileManager();
	assert(file != nullptr);

	m_Path.ReplaceAll("\\", "/");
	//Relative Path
	if (m_Path.StartWith('/'))
	{
		m_AbsPath = file->EngineDir() + str;
	}
	else
	{
		m_AbsPath = str;
	}
}

LPath::LPath(const LString& str) :m_Path(str)
{
	static FileSubsystem* fileSub = gEngine->GetSubsystem<FileSubsystem>();
	assert(fileSub != nullptr);
	static IPlatformFileManager* file = fileSub->GetPlatformFileManager();
	assert(file != nullptr);

	m_Path.ReplaceAll("\\", "/");
	//Relative Path
	if (m_Path.StartWith('/'))
	{
		m_AbsPath = file->EngineDir() + str;
	}
	else
	{
		m_AbsPath = str;
	}
}

luna::LString luna::LPath::GetFileNameWithExt() const
{
	return "";
}

bool luna::LPath::IsRelative() const
{
	return m_Path.StartWith("/");
}

