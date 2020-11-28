#include "platform_file.h"
#include "core/object/object.h"
#include <boost/filesystem.hpp>

using namespace luna;

int64_t IFile::Size()
{
	return 0;
}

LSharedPtr<luna::LFile> WindowsFileManager::Open(const LPath& path, int mode)
{

	auto file = boost::make_shared<LFile>();
	file->m_File.open(path.AsString(),mode);
	if (file->m_File.fail())
	{
		LogError(E_Core, g_Failed, "open file failed");
	}
	return NULL;
}

bool WindowsFileManager::IsExists(const LPath& path)
{	
	return boost::filesystem::exists(*path.AsString());
}

bool WindowsFileManager::IsDirctory(const LPath& path)
{
	return boost::filesystem::is_directory(*path.AsString());
}

bool WindowsFileManager::IsFile(const LPath& path)
{
	return boost::filesystem::is_regular_file(*path.AsString());
}

bool WindowsFileManager::DeleteFile(const LPath& path)
{
	return boost::filesystem::remove(*path.AsString());
}

bool WindowsFileManager::CreateDirectory(const LPath& path)
{
	return boost::filesystem::create_directory(*path.AsString());
}

bool WindowsFileManager::CreateFile(const LPath& path)
{
	throw std::logic_error("The method or operation is not implemented.");
}

const luna::LString& WindowsFileManager::EngineDir()
{
	return m_EngineDir;
}

bool WindowsFileManager::InitFileManager()
{
	TCHAR tempPath[1000];
	GetModuleFileName(NULL, tempPath, MAX_PATH);
	
	m_ApplicationPath = LString(tempPath);
	m_ApplicationPath.ReplaceAll("\\", "/");
	
	size_t pos = m_ApplicationPath.FindLast("/");
	if (pos != LString::npos)
	{
		m_ApplicationPath.EraseAt(pos);
		m_EngineDir = m_ApplicationPath.Substr(0, pos);		
	}

	return true;
}
