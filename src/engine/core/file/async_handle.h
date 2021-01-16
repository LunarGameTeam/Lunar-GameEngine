#pragma once

#include "file.h"
#include <boost/function.hpp>

namespace luna
{

enum class AsyncState
{
	PendingQueue = 0,
	Running = 1,
	Finished = 2
};

using FileAsyncCallback = boost::function<void(LSharedPtr<LFile>)>;
/*!
 * \class FileAsyncHandle
 *
 * \brief 文件异步加载Handle
 *
 * \author isAk wOng
 * 
 */
class FileAsyncHandle
{
public:
	LString path;
	FileAsyncCallback callback;
	LSharedPtr<LFile> file;
	AsyncState state;
};

}