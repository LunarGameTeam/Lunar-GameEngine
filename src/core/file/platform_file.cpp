#include "platform_file.h"
#include "core/framework/luna_core.h"
#include "core/object/base_object.h"
#include "core/file/platform_module.h"

#include <filesystem>

namespace luna
{
LSharedPtr<LFileStream> WindowsFileManager::OpenAsStream(const LPath &path, OpenMode mode)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	file->mFileStream.open(path.AsStringAbs(), (int)mode);
	if (file->mFileStream.fail())
	{
		LogError("Core", "Open File: {0} Failed", path.AsString().c_str());
		return NULL;
	}
	return file;
}

bool WindowsFileManager::ReadStringFromFile(const LPath &path, LString &res)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	file->mFileStream.open(path.AsStringAbs(), (int)OpenMode::In);

	if (file->mFileStream.fail())
	{
		LogError("Core", "Open File: {0} Failed", path.AsString().c_str());
		return false;
	}
	std::stringstream stream;
	stream << file->mFileStream.rdbuf();
	res.Assign(stream.str());
	return true;
}

bool WindowsFileManager::IsExists(const LPath &path)
{
	return std::filesystem::exists(*path.AsString());
}

bool WindowsFileManager::IsDirctory(const LPath &path)
{
	return std::filesystem::is_directory(*path.AsString());
}

bool WindowsFileManager::IsFile(const LPath &path)
{
	return std::filesystem::is_regular_file(*path.AsString());
}

bool WindowsFileManager::DeleteFile(const LPath &path)
{
	return std::filesystem::remove(*path.AsString());
}

bool WindowsFileManager::CreateDirectory(const LPath &path)
{
	return std::filesystem::create_directory(*path.AsString());
}

bool WindowsFileManager::CreateFile(const LPath &path)
{
	throw std::logic_error("The method or operation is not implemented.");
}

const LString &WindowsFileManager::EngineDir()
{
	return m_root_dir;
}

const LString& WindowsFileManager::ApplicationDir()
{
	return m_exe_path;
}

bool WindowsFileManager::GetFileInfo(const LPath &path, LFileInfo &result, bool recursive /*= false*/)
{
	std::filesystem::path p(path.AsStringAbs().c_str());
	if (!std::filesystem::exists(p))
	{
		return false;
	}
	std::filesystem::directory_entry file(p);
	result.mIsFolder = file.is_directory();
	result.mPath = file.path().string();
#ifdef _WIN32
	result.mPath.ReplaceAll("\\", "/");
#endif
	result.mName = file.path().filename().string();
	for (const auto &entry : std::filesystem::directory_iterator(p))
	{
		LString path = entry.path().string();
#ifdef _WIN32
		path.ReplaceAll("\\", "/");
#endif
		LFileInfo &info = result.mFolderContents[path];
		info.mParent = &result;
		info.mEnginePath = LPath::ConvertToEnginePath(path);
		info.mDepth = result.mDepth + 1;
		info.mIsFolder = entry.is_directory();
		info.mName = entry.path().filename().string();
		info.mPath = path;
		if (info.mIsFolder && recursive)
		{
			GetFileInfo(LPath(info.mPath), info, recursive);
			info.mIsFolderInit = true;
		}
	}
	if (result.mIsFolder)
		result.mIsFolderInit = true;
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
		auto &info = result.mFolderContents[path];
		info.mEnginePath = LPath::ConvertToEnginePath(path);
		info.mIsFolder = entry.is_directory();
		info.mParent = &result;
		info.mDepth = result.mDepth + 1;
		info.mName = entry.path().filename().string(); 
		info.mPath = path;
	}
	return true;
}

bool WindowsFileManager::WriteStringToFile(const LPath &path, const LString &res)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	file->mFileStream.open(path.AsStringAbs(), (int)OpenMode::Out | (int)OpenMode::Trunc);

	if (file->mFileStream.fail())
	{
		LogError("Core", "open file failed");
		return false;
	}
	file->mFileStream << res;
	file->mFileStream.flush();
	file->mFileStream.close();
	return true;
}

bool WindowsFileManager::DisposeFileManager()
{
	m_pending_lock.lock();
	io_loop = false;
	m_pending_lock.unlock();

	delete m_io_thread;
	m_io_thread = nullptr;
	m_pending_lock.lock();
	while (!m_pending_queue.empty()) m_pending_queue.pop();
	m_pending_lock.unlock();

	return true;
}

LSharedPtr<LFile> WindowsFileManager::WriteSync(const LPath &path, const LVector<byte> &data)
{
	HANDLE file_handle = ::CreateFileA(path.AsStringAbs().c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, NULL, NULL);
	LSharedPtr<LFile> file = MakeShared<LFile>();
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		LogError("Core", "Open File: {0} Failed", path.AsString().c_str());
		return file;
	}
	DWORD size = (DWORD)data.size();
	file->mData = data;
	file->mPath = path.AsStringAbs();
	DWORD actual_size = 0;
	::WriteFile(file_handle, file->mData.data(), size, &actual_size, NULL);
	file->mIsOk = true;

	return LSharedPtr<LFile>();
}

LSharedPtr<FileAsyncHandle> WindowsFileManager::ReadAsync(const LPath &path, FileAsyncCallback callback)
{
	LSharedPtr<FileAsyncHandle> async_handle = MakeShared<FileAsyncHandle>();
	//组装异步Handle
	async_handle->mCallback = callback;
	async_handle->mFile = MakeShared<LFile>();
	async_handle->mAsyncState = AsyncState::PendingQueue;
	async_handle->mPath = path.AsStringAbs();

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
		LogError("Core", "Open File: {0} Failed", path.AsString().c_str());
		return file;
	}
	DWORD size = GetFileSize(file_handle, NULL);
	file->mData.resize(size);
	file->mPath = path.AsStringAbs();
	DWORD actual_size = 0;
	::ReadFile(file_handle, file->mData.data(), size, &actual_size, NULL);
	file->mIsOk = true;
	return file;
}

VOID CALLBACK WindowsFileManager::FileCompleteCallback(
	DWORD dwErrorCode,                // 对于此次操作返回的状态
	DWORD dwNumberOfBytesTransfered,  // 告诉已经操作了多少字节,也就是在IRP里的Infomation
	LPOVERLAPPED lpOverlapped         // 这个数据结构
)
{
	static WindowsFileManager *instance = (WindowsFileManager *)gEngine->GetModule<PlatformModule>()->GetPlatformFileManager();

	LSharedPtr<FileAsyncHandle> handle = instance->m_running_handles[lpOverlapped];

	handle->mCallback(handle);
	handle->mAsyncState = AsyncState::Finished;

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

			HANDLE file_handle = ::CreateFileA(async_handle->mPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if (file_handle == INVALID_HANDLE_VALUE)
				goto END;

			LSharedPtr<LFile> file = async_handle->mFile;
			DWORD size = GetFileSize(file_handle, NULL);
			DWORD actual_size = 0;
			OVERLAPPED *overlap = new OVERLAPPED();
			memset(overlap, 0, sizeof(OVERLAPPED));

			file->mData.resize(size);
			file->mPath = async_handle->mPath;

			//创建overlap事件
			async_handle->mAsyncState = AsyncState::Running;

			bool rc = ReadFileEx(file_handle, file->mData.data(), size, overlap, WindowsFileManager::FileCompleteCallback);	//进入alterable
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
	m_exe_path = LString(tempPath);

	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	m_root_dir = LString(tempPath);
	if (m_root_dir.Find("\\bin") != luna::LString::npos)
	{
		m_root_dir.Replace("\\bin", "");
	}

	m_root_dir.ReplaceAll("\\", "/");
	m_exe_path.ReplaceAll("\\", "/");

	SetCurrentDirectory(m_root_dir.c_str());
	SetDllDirectoryA(m_root_dir + "/bin");


	m_io_thread = new LThread(std::bind(&WindowsFileManager::IO_Thread, this));

	return true;
}
}