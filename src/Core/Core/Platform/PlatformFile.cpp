#include "Core/Platform/PlatformFile.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Object/BaseObject.h"
#include "Core/Platform/PlatformModule.h"

#include <filesystem>
#include "Core/Platform/File.h"
#include "windows.h"


#undef CreateFile
#undef CreateDirectory
#undef DeleteFile

namespace luna
{


LSharedPtr<LFileStream> WindowsFileManager::OpenAsStream(const LPath &path, OpenMode mode)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	
	file->mFileStream.open(mEngineDir + "/" + path.AsString(), (int)mode);
	if (file->mFileStream.fail())
	{
		file->mFileStream.open(mProjectDir + "/" + path.AsString(), (int)mode);		
	}
	return file;
}

void WindowsFileManager::SetProjectDir(const LString& path)
{
	mProjectDir = path;
}

bool WindowsFileManager::ReadStringFromFile(const LPath &path, LString &res)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	file->mFileStream.open(mEngineDir + "/" + path.AsString(), (int)OpenMode::In);

	if (file->mFileStream.fail())
	{
		file->mFileStream.open(mProjectDir + "/" + path.AsString(), (int)OpenMode::In);
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

bool WindowsFileManager::Delete_File(const LPath &path)
{
	return std::filesystem::remove(*path.AsString());
}

bool WindowsFileManager::Create_Directory(const LPath &path)
{
	return std::filesystem::create_directory(*path.AsString());
}

bool WindowsFileManager::Create_File(const LPath &path)
{
	throw std::logic_error("The method or operation is not implemented.");
}

const LString &WindowsFileManager::EngineDir()
{
	return mEngineDir;
}

const LString& WindowsFileManager::ApplicationDir()
{
	return mExecPath;
}

bool WindowsFileManager::GetFileInfo(const LPath &path, LFileInfo &result, bool recursive /*= false*/)
{
	std::filesystem::path p(path.AsEnginePathString().c_str());
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
	std::filesystem::path p(path.AsEnginePathString().c_str());
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

const luna::LString& WindowsFileManager::ProjectDir()
{
	return mProjectDir;
}

bool WindowsFileManager::WriteStringToFile(const LPath &path, const LString &res)
{
	LSharedPtr<LFileStream> file = MakeShared<LFileStream>();
	file->mFileStream.open(path.AsEnginePathString(), (int)OpenMode::Out | (int)OpenMode::Trunc);

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
	mPendingLock.lock();
	mIOLooping = false;
	mPendingLock.unlock();

	delete mIOThread;
	mIOThread = nullptr;
	mPendingLock.lock();
	while (!mPendingQueue.empty()) mPendingQueue.pop();
	mPendingLock.unlock();

	return true;
}

LSharedPtr<LFile> WindowsFileManager::WriteSync(const LPath &path, const LArray<byte> &data)
{
	HANDLE fileHandle = ::CreateFileA(path.AsProjectPathString(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
	LSharedPtr<LFile> file = MakeShared<LFile>();
	file->mPath = path.AsProjectPathString();
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		fileHandle = ::CreateFileA(path.AsEnginePathString(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
		file->mPath = path.AsEnginePathString();
	}
	DWORD size = (DWORD)data.size();
	file->mData = data;
	DWORD actualSize = 0;
	bool result = ::WriteFile(fileHandle, file->mData.data(), size, &actualSize, NULL);
	::CloseHandle(fileHandle);
	file->mIsOk = result;
	assert(result);
	return file;
}

LSharedPtr<FileAsyncHandle> WindowsFileManager::ReadAsync(const LPath &path, FileAsyncCallback callback)
{
	LSharedPtr<FileAsyncHandle> async_handle = MakeShared<FileAsyncHandle>();
	//组装异步Handle
	async_handle->mCallback = callback;
	async_handle->mFile = MakeShared<LFile>();
	async_handle->mAsyncState = AsyncState::PendingQueue;
	async_handle->mPath = path.AsEnginePathString();

	mPendingLock.lock();
	mPendingQueue.push(async_handle);
	mPendingLock.unlock();

	return async_handle;
}

LSharedPtr<LFile> WindowsFileManager::ReadSync(const LPath &path)
{
	HANDLE fileHandle = ::CreateFileA(path.AsProjectPathString(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	LSharedPtr<LFile> file = MakeShared<LFile>();
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		fileHandle = ::CreateFileA(path.AsEnginePathString(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			LogError("Core", "Open File: {0} Failed", path.AsString().c_str());
			return file;
		}		
	}
	DWORD size = GetFileSize(fileHandle, NULL);
	file->mData.resize(size);
	file->mPath = path.AsEnginePathString();
	DWORD actualSize = 0;
	::ReadFile(fileHandle, file->mData.data(), size, &actualSize, NULL);
	file->mIsOk = true;
	::CloseHandle(fileHandle);
	return file;
}

VOID CALLBACK WindowsFileManager::FileCompleteCallback(
	DWORD dwErrorCode,                // 对于此次操作返回的状态
	DWORD dwNumberOfBytesTransfered,  // 告诉已经操作了多少字节,也就是在IRP里的Infomation
	LPOVERLAPPED lpOverlapped         // 这个数据结构
)
{
	static WindowsFileManager *instance = (WindowsFileManager *)gEngine->GetModule<PlatformModule>()->GetPlatformFileManager();

	LSharedPtr<FileAsyncHandle> handle = instance->mRunningHandles[lpOverlapped];

	handle->mCallback(handle);
	handle->mAsyncState = AsyncState::Finished;

	instance->mRunningLock.lock();
	instance->mRunningHandles.erase(lpOverlapped);
	delete lpOverlapped;
	instance->mRunningLock.unlock();
}

void WindowsFileManager::IO_Thread()
{
	while (mIOLooping)
	{
		{
			if (mPendingQueue.empty())
				goto END;

			//取AsyncHandle并进行处理
			LSharedPtr<FileAsyncHandle> async_handle = mPendingQueue.front();
			mPendingLock.lock();
			mPendingQueue.pop();
			mPendingLock.unlock();

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

			mRunningLock.lock();
			mRunningHandles[overlap] = async_handle;
			mRunningLock.unlock();
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
	mExecPath = LString(tempPath);

	GetCurrentDirectory(MAX_PATH, tempPath); //获取程序的当前目录
	mEngineDir = LString(tempPath);
	if (mEngineDir.Find("\\bin") != luna::LString::npos)
	{
		mEngineDir.Replace("\\bin", "");
	}

	mEngineDir.ReplaceAll("\\", "/");
	mExecPath.ReplaceAll("\\", "/");

	SetCurrentDirectory(mEngineDir.c_str());
	SetDllDirectoryA(mEngineDir + "/bin");


	mIOThread = new LThread(std::bind(&WindowsFileManager::IO_Thread, this));

	return true;
}

}