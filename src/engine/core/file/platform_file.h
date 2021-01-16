#pragma once

#include "core/private_core.h"
#include "path.h"
#include "file.h"
#include "async_handle.h"
#include "core/thread/thread.h"

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/lockfree/queue.hpp>

namespace luna
{

template<typename... Args>
auto Bind(Args && ...args)
{
	return boost::bind(args);
}


enum class OpenMode : int
{
	In = std::ios::in,
	Out = std::ios::out,
	App = std::ios::app,
	Trunc = std::ios::trunc,
};

/*!
 * \class IPlatformFileManager
 *
 * \brief 平台文件操作接口，不同平台需要继承并实现所有接口
 *
 * \author isAk wOng
 *
 */
class IPlatformFileManager
{
public:
	virtual bool InitFileManager() = 0;
	virtual bool DisposeFileManager() = 0;

public:
	virtual LSharedPtr<LFile> ReadSync(const LPath &path) = 0;
	virtual LSharedPtr<FileAsyncHandle> ReadAsync(const LPath &path, FileAsyncCallback callback) = 0;

	virtual LSharedPtr<LFile> WriteSync(const LPath &path, const LVector<byte> &data) = 0;

public:
	virtual LSharedPtr<LFileStream> OpenAsStream(const LPath &path, OpenMode mode) = 0;
	virtual bool ReadStringFromFile(const LPath &path, LString &res) = 0;
	virtual bool IsExists(const LPath &path) = 0;
	virtual bool IsDirctory(const LPath &path) = 0;
	virtual bool IsFile(const LPath &path) = 0;
	virtual bool DeleteFile(const LPath &path) = 0;
	virtual bool CreateDirectory(const LPath &path) = 0;
	virtual bool CreateFile(const LPath &path) = 0;

	virtual const LString &EngineDir() = 0;
protected:
	LString m_EngineDir;
	LString m_ApplicationPath;
};

/*!
 * \class WindowsFileManager
 *
 * \brief Windows平台的FileManager实现
 *
 * \author isAk wOng
 *
 */
class WindowsFileManager : public IPlatformFileManager
{
public:
	bool InitFileManager()override;
	bool DisposeFileManager() override;

	LSharedPtr<LFile> ReadSync(const LPath &path) override;
	LSharedPtr<LFile> WriteSync(const LPath &path, const LVector<byte> &data) override;
	LSharedPtr<FileAsyncHandle> ReadAsync(const LPath &path, FileAsyncCallback callback) override;
	LSharedPtr<LFileStream> OpenAsStream(const LPath &path, OpenMode mode) override;
	bool ReadStringFromFile(const LPath &path, LString &res) override;
	bool IsExists(const LPath &path) override;
	bool IsDirctory(const LPath &path) override;
	bool IsFile(const LPath &path) override;
	bool DeleteFile(const LPath &path) override;
	bool CreateDirectory(const LPath &path) override;
	bool CreateFile(const LPath &path) override;
	const LString &EngineDir() override;

private:
	boost::mutex m_pending_lock;
	boost::mutex m_running_lock;

	std::queue<LSharedPtr<FileAsyncHandle>> m_pending_queue;
	LMap<OVERLAPPED *, LSharedPtr<FileAsyncHandle>> m_running_handles;

	void IO_Thread();
	static void FileCompleteCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

	LThread *m_io_thread;



};

}