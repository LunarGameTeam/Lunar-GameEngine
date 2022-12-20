#pragma once

#include "core/object/base_object.h"
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

class CORE_API LBasicAsset : public LObject
{
	RegisterTypeEmbedd(LBasicAsset, LObject)
public:	
	LBasicAsset();
	virtual ~LBasicAsset();

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
	LPath mAssetPath;//统一的Asset URL path
};

class CORE_API LBinaryAsset : public LBasicAsset
{
	RegisterTypeEmbedd(LBinaryAsset, LBasicAsset)
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

class CORE_API LTextAsset : public LBasicAsset
{
	RegisterTypeEmbedd(LTextAsset, LBasicAsset)
public:
	LTextAsset();

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
