#pragma once


#include "core/object/base_object.h"
#include "path.h"
#include <fstream>

namespace luna
{
class WindowsFileManager;

struct CORE_API LFileInfo
{
	LString mPath;
	LString mName;
	LString mEnginePath;
	LFileInfo* mParent = nullptr;
	int mDepth = 0;
	bool mIsFolder = false;
	bool mIsFolderInit = false;	
	LMap<LString, LFileInfo> mFolderContents;
};

struct CORE_API LFile
{
public:
	~LFile()
	{

	}
	const LString &GetPath()
	{
		return mPath;
	}
	const LArray<byte> &GetData()
	{
		return mData;
	}
	bool IsOk()
	{
		return mIsOk;
	}
private:
	LString mPath;
	LArray<byte> mData;
	bool mIsOk = false;
	friend class WindowsFileManager;
};

/*!
 * \class LFileStream
 *
 * \brief 对fstream简单的包装类
 *
 * \author isAk wOng
 *
 */
class CORE_API LFileStream
{
public:
	LFileStream() = default;
	LFileStream(const LFileStream&) = delete;
	LFileStream& operator=(const LFileStream&) = delete;

	~LFileStream()
	{
		if (mFileStream.is_open())
		{
			mFileStream.flush();
			mFileStream.close();
		}
	}
	bool Ready()
	{
		return mReady;
	}
	int64_t Tell() const
	{
		return mPos;
	}
	void Seek(int64_t pos)
	{
		mPos = pos;
	}
	virtual void Read(byte *Destination, int64_t BytesToRead);
	virtual void Write(const byte *Source, int64_t BytesToWrite);
	virtual void Flush(const bool bFullFlush = false);
	virtual int64_t Size();

	virtual void ReadToVector(LArray<byte> &dest);
	virtual void WriteFromVector(LArray<byte> &dest);

private:
	int mMode;
	int64_t mPos;
	std::fstream mFileStream;
	bool mReady = false;

	friend class WindowsFileManager;
};

RegisterType(CORE_API, LFileInfo, LFileInfo)


}

namespace luna::binding{

template<>
struct binding_converter<LFileInfo*> : native_converter<LFileInfo> {};
template<>
struct binding_proxy<LFileInfo> : native_binding_proxy<LFileInfo> {};

}