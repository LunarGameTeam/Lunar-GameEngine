#pragma once


#include "Core/Memory/Ptr.h"
#include "Core/Foundation/Container.h"
#include "Core/Foundation/Log.h"
#include "Core/Platform/File.h"
#include <functional>

namespace luna
{
enum class AsyncState
{
	PendingQueue = 0, //加载队列中
	Running = 1,//系统读取中
	Finished = 2,//加载成功
	Cancle = 3, //取消掉
	Release = 4, //加载成功，然后释放掉
	Error = 5,//加载失败
};

class FileAsyncHandle;

using FileAsyncCallback = std::function<void(LSharedPtr<FileAsyncHandle>)>;

/*!
 * \class FileAsyncHandle
 *
 * \brief 文件异步加载Handle
 *
 * \author isAk wOng
 *
 */
class CORE_API FileAsyncHandle
{
public:
	void Cancel()
	{
		mAsyncState = AsyncState::Cancle;
	}
	inline bool IsRunning()
	{
		return mAsyncState == AsyncState::Running;
	}
	inline bool IsFinished()
	{
		return mAsyncState == AsyncState::Finished;
	}
	void ReleaseFile()
	{
		mAsyncState = AsyncState::Release;
		mFile.reset();
	}
private:
	LString mPath;
	FileAsyncCallback mCallback;
	LSharedPtr<LFile> mFile;
	AsyncState mAsyncState;

	friend class WindowsFileManager;
};
}