#include "platform_file.h"
#include "core/object/object.h"
#include "core/file/file_subsystem.h"

#include <boost/filesystem.hpp>
#include <filesystem>

namespace luna
{

bool io_loop = true;

LSharedPtr<LFileStream> WindowsFileManager::OpenAsStream(const LPath &path, OpenMode mode)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	file->m_file.open(path.AsStringAbs(), (int)mode);
	if (file->m_file.fail())
	{
		LogError(E_Core, g_Failed, "open file failed");
		return NULL;
	}
	return file;
}

bool WindowsFileManager::ReadStringFromFile(const LPath &path, LString &res)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
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


bool WindowsFileManager::GetFileInfo(const LPath &path, LFileInfo &result, bool recursive /*= false*/)
{
	std::filesystem::path p(path.AsStringAbs().c_str());
	if (!std::filesystem::exists(p))
	{
		return false;
	}
	std::filesystem::directory_entry file(p);
	result.is_folder = file.is_directory();
	result.path = file.path().string();
#ifdef _WIN32
	result.path.ReplaceAll("\\", "/");
#endif
	for (const auto &entry : std::filesystem::directory_iterator(p))
	{
		LString path = entry.path().string();
#ifdef _WIN32
		path.ReplaceAll("\\", "/");
#endif
		auto &info = result.folder_contents[path];
		info.is_folder = entry.is_directory();
		info.path = path;
		if (info.is_folder && recursive)
		{
			GetFileInfo(LPath(info.path), info, recursive);
			info.is_folder_contents_init = true;
		}
	}
	if (result.is_folder)
		result.is_folder_contents_init = true;
	return true;
}

bool WindowsFileManager::GetFileInfoRecursive(const LPath &path, LFileInfo &result, bool recursive /*= false*/)
{
	std::filesystem::path p(path.AsStringAbs().c_str());
	if (!std::filesystem::exists(p))
	{
		return false;
	}
	std::filesystem::directory_entry file(p);
	for (const auto &entry : std::filesystem::recursive_directory_iterator(p))
	{
		LString path = entry.path().string();
#ifdef _WIN32
		path.ReplaceAll("\\", "/");
#endif
		auto &info = result.folder_contents[path];
		info.is_folder = entry.is_directory();
		info.path = path;
	}
	return true;
}

bool WindowsFileManager::WriteStringToFile(const LPath &path, const LString &res)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	file->m_file.open(path.AsStringAbs(), (int)OpenMode::Out | (int)OpenMode::Trunc);

	if (file->m_file.fail())
	{
		LogError(E_Core, g_Failed, "open file failed");
		return false;
	}
	file->m_file << res;
	file->m_file.flush();
	file->m_file.close();
	return true;
}

bool WindowsFileManager::DisposeFileManager()
{
	io_loop = false;

	m_pending_lock.lock();
	while (!m_pending_queue.empty()) m_pending_queue.pop();
	m_pending_lock.unlock();

	return true;
}

LSharedPtr<LFile> WindowsFileManager::WriteSync(const LPath &path, const LVector<byte> &data)
{
	return LSharedPtr<LFile>();
}

LSharedPtr<FileAsyncHandle> WindowsFileManager::ReadAsync(const LPath &path, FileAsyncCallback callback)
{
	LSharedPtr<FileAsyncHandle> async_handle = MakeShared<FileAsyncHandle>();
	//组装异步Handle
	async_handle->callback = callback;
	async_handle->file = MakeShared<LFile>();
	async_handle->state = AsyncState::PendingQueue;
	async_handle->path = path.AsStringAbs();

	m_pending_lock.lock();
	m_pending_queue.push(async_handle);
	m_pending_lock.unlock();

	return async_handle;
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
	file->path = path.AsStringAbs();
	DWORD actual_size = 0;
	::ReadFile(file_handle, file->data.data(), size, &actual_size, NULL);
	file->is_ok = true;
	return file;
}

VOID CALLBACK WindowsFileManager::FileCompleteCallback(
	DWORD dwErrorCode,                // 对于此次操作返回的状态
	DWORD dwNumberOfBytesTransfered,  // 告诉已经操作了多少字节,也就是在IRP里的Infomation
	LPOVERLAPPED lpOverlapped         // 这个数据结构
)
{
	static WindowsFileManager *instance = (WindowsFileManager *)gEngine->GetSubsystem<FileSubsystem>()->GetPlatformFileManager();

	LSharedPtr<FileAsyncHandle> handle = instance->m_running_handles[lpOverlapped];

	handle->callback(handle);
	handle->state = AsyncState::Finished;

	instance->m_running_lock.lock();
	instance->m_running_handles.erase(lpOverlapped);
	delete lpOverlapped;
	instance->m_running_lock.unlock();
}

void WindowsFileManager::IO_Thread()
{
	while (io_loop)
	{
		
		{
			if (m_pending_queue.empty())
				goto END;

			//取AsyncHandle并进行处理
			LSharedPtr<FileAsyncHandle> async_handle = m_pending_queue.front();
			m_pending_lock.lock();
			m_pending_queue.pop();
			m_pending_lock.unlock();

			HANDLE file_handle = ::CreateFileA(async_handle->path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if (file_handle == INVALID_HANDLE_VALUE)
				goto END;

			LSharedPtr<LFile> file = async_handle->file;
			DWORD size = GetFileSize(file_handle, NULL);
			DWORD actual_size = 0;
			OVERLAPPED *overlap = new OVERLAPPED();
			memset(overlap, 0, sizeof(OVERLAPPED));

			file->data.resize(size);
			file->path = async_handle->path;

			//创建overlap事件
			async_handle->state = AsyncState::Running;

			bool rc = ReadFileEx(file_handle, file->data.data(), size, overlap, WindowsFileManager::FileCompleteCallback);	//进入alterable
			if (!rc)
				goto END;

			m_running_lock.lock();
			m_running_handles[overlap] = async_handle;
			m_running_lock.unlock();
		}

	END:
		SleepEx(0, TRUE);
		continue;
	}
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

	m_io_thread = new LThread(boost::bind(&WindowsFileManager::IO_Thread, this));
	return true;
}

}