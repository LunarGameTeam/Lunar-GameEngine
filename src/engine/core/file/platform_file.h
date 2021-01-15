#pragma once

#include "core/private_core.h"
#include "path.h"
#include "file.h"
#include "async_handle.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace luna
{

template<typename... Args>
auto Bind(Args&& ...args)
{
	return boost::bind(args);
}

using FileAsyncCallback = boost::function<void(LSharedPtr<LFile>)>;

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

public:
	virtual LSharedPtr<LFile> ReadSync(const LPath &path) = 0;
	virtual FileAsyncHandle ReadAsync(const LPath &path, FileAsyncCallback callback) = 0;

	virtual LSharedPtr<LFile> WriteSync(const LPath &path, const LVector<byte>& data) = 0;

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
	
	LSharedPtr<LFile> ReadSync(const LPath &path) override;
	LSharedPtr<LFile> WriteSync(const LPath &path, const LVector<byte> &data) override;
	FileAsyncHandle ReadAsync(const LPath &path, FileAsyncCallback callback) override;
	LSharedPtr<LFileStream> OpenAsStream(const LPath &path, OpenMode mode) override;
	bool ReadStringFromFile(const LPath &path, LString &res) override;
	bool IsExists(const LPath &path) override;
	bool IsDirctory(const LPath &path) override;
	bool IsFile(const LPath &path) override;
	bool DeleteFile(const LPath &path) override;
	bool CreateDirectory(const LPath &path) override;
	bool CreateFile(const LPath &path) override;
	const LString &EngineDir() override;

};

}