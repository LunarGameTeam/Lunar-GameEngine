#include "platform_file.h"
#include "core/object/object.h"
#include <boost/filesystem.hpp>
namespace luna
{


LSharedPtr<LFileStream> WindowsFileManager::OpenAsStream(const LPath &path, OpenMode mode)
{
	LSharedPtr<LFileStream> file = boost::make_shared<LFileStream>();
	file->m_file.open(path.AsStringAbs(),(int)mode);
	if (file->m_file.fail())
	{
		LogError(E_Core, g_Failed, "open file failed");
		return NULL;
	}
	return file;
}

bool WindowsFileManager::ReadStringFromFile(const LPath &path, LString& res)
{
	LSharedPtr<LFileStream> file = boost::make_shared<LFileStream>();
	file->m_file.open(path.AsStringAbs(), (int)OpenMode::In);

	if (file->m_file.fail())
	{
		LogError(E_Core, g_Failed, "open file failed");
		return false;
	}
	std::stringstream stream;
	stream << file->m_file.rdbuf();
	res.Assign(stream.str());
	return true;
}

bool WindowsFileManager::IsExists(const LPath &path)
{
	return boost::filesystem::exists(*path.AsString());
}

bool WindowsFileManager::IsDirctory(const LPath &path)
{
	return boost::filesystem::is_directory(*path.AsString());
}

bool WindowsFileManager::IsFile(const LPath &path)
{
	return boost::filesystem::is_regular_file(*path.AsString());
}

bool WindowsFileManager::DeleteFile(const LPath &path)
{
	return boost::filesystem::remove(*path.AsString());
}

bool WindowsFileManager::CreateDirectory(const LPath &path)
{
	return boost::filesystem::create_directory(*path.AsString());
}

bool WindowsFileManager::CreateFile(const LPath &path)
{
	throw std::logic_error("The method or operation is not implemented.");
}

const LString &WindowsFileManager::EngineDir()
{
	return m_EngineDir;
}

LSharedPtr<luna::LFile> WindowsFileManager::WriteSync(const LPath &path, const LVector<byte> &data)
{
	throw std::logic_error("The method or operation is not implemented.");
}

FileAsyncHandle WindowsFileManager::ReadAsync(const LPath &path, FileAsyncCallback callback)
{
	auto file = ReadSync(path);
	callback(file);
	return FileAsyncHandle();
}

LSharedPtr<LFile> WindowsFileManager::ReadSync(const LPath &path)
{
	HANDLE file_handle = ::CreateFileA(path.AsStringAbs().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	LSharedPtr<LFile> file = MakeShared<LFile>();
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		return file;
	}
	DWORD size = GetFileSize(file_handle, NULL);
	file->data.resize(size);
	file->full_path = path.AsStringAbs();
	DWORD actual_size = 0;
	::ReadFile(file_handle, file->data.data(), size, &actual_size, NULL);
	file->is_ok = true;
	return file;
}

bool WindowsFileManager::InitFileManager()
{
	TCHAR tempPath[1000];
	GetModuleFileName(NULL, tempPath, MAX_PATH);
	m_ApplicationPath = LString(tempPath);
	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	m_EngineDir = LString(tempPath);

	m_EngineDir.ReplaceAll("\\", "/");
	m_ApplicationPath.ReplaceAll("\\", "/");

	size_t pos = m_ApplicationPath.FindLast("/");
	return true;
}

}