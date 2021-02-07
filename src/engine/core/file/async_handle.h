#pragma once

#include "core/private_core.h"
#include "core/memory/ptr.h"
#include "core/misc/container.h"
#include "core/log/log.h"
#include "file.h"
#include <boost/function.hpp>

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

using FileAsyncCallback = boost::function<void(LSharedPtr<FileAsyncHandle>)>;

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
		state = AsyncState::Cancle;
	}
	inline bool IsRunning()
	{
		return state == AsyncState::Running;
	}
	inline bool IsFinished()
	{
		return state == AsyncState::Finished;
	}
	void ReleaseFile()
	{
		state = AsyncState::Release;
		file.reset();
	}
private:
	LString path;
	FileAsyncCallback callback;
	LSharedPtr<LFile> file;
	AsyncState state;

	friend class WindowsFileManager;
};

}