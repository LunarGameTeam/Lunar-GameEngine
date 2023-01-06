#pragma once

#include "core/object/shared_object.h"
#include "core/memory/ptr.h"
#include "core/foundation/string.h"
#include "core/foundation/container.h"
#include "core/file/async_handle.h"
#include "core/file/file.h"
#include "core/file/path.h"
#include "core/serialization/dict_data.h"

namespace luna
{
class AssetModule;

class CORE_API Asset : public SharedObject
{
	RegisterTypeEmbedd(Asset, SharedObject)
public:	
	Asset();
	virtual ~Asset();

public:
	//Asset资源读入到内存时回调
	virtual void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file);
	//Asset资源写入到磁盘时回调	
	virtual void OnAssetFileWrite(LSharedPtr<Dictionary> meta, LArray<byte>& data);
	//资源Release
	virtual void OnAssetRelease();

	virtual void OnLoad() {};
	

	LString GetAssetPath();
	
private:
	friend class AssetModule;
	LString mAssetPath;//统一的Asset URL path
};

class CORE_API LBinaryAsset : public Asset
{
	RegisterTypeEmbedd(LBinaryAsset, Asset)
public:
	LBinaryAsset();
	virtual ~LBinaryAsset()
	{

	}
	LSharedPtr<LFile> GetData() 
	{
		return mData;
	};
public:
	virtual void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file);

private:
	LSharedPtr<LFile> mData;
};

class CORE_API TextAsset : public Asset
{
	RegisterTypeEmbedd(TextAsset, Asset)
public:
	TextAsset();

	const LString &GetContent() const
	{
		return mContent;
	}

public:
	virtual void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file);	

protected:
	LString mContent;
};

}
