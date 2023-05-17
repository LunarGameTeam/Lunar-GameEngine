#pragma once

#include "Core/Object/SharedObject.h"
#include "Core/Foundation/String.h"
#include "Core/Foundation/Container.h"
#include "Core/Platform/File.h"
#include "Core/Platform/Path.h"
#include "Core/Memory/Ptr.h"
#include "Core/Serialization/JsonDict.h"

namespace luna
{
class AssetModule;

template<typename T>
using TAssetPtrArray = std::vector<SharedPtr<T>>;

class CORE_API Asset : public SharedObject
{
	RegisterTypeEmbedd(Asset, SharedObject)
public:	
	Asset();
	virtual ~Asset();

public:
	//Asset资源读入到内存时回调
	virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file);
	//Asset资源写入到磁盘时回调	
	virtual void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data);
	//资源Release
	virtual void OnAssetRelease();

	virtual void OnLoad() {};
	

	LString GetAssetPath();
	
private:
	friend class AssetModule;
	LString mAssetPath;//统一的Asset URL path

protected:
	void CopyPointToByteArray(const void* dataSrc, size_t dataLength, LArray<byte>& dataDst);
};

class CORE_API LBinaryAsset : public Asset
{
	RegisterTypeEmbedd(LBinaryAsset, Asset)
public:
	LBinaryAsset();
	virtual ~LBinaryAsset()
	{

	}
	LSharedPtr<LFile> GetFileData() 
	{
		return mData;
	};
public:
	virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file);

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
	virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file);	

protected:
	LString mContent;
};

template<typename T>
struct static_type<SharedPtr<T>>
{
	static LType* Init()
	{
		LType* type = NewTemplateType<T>("SharedPtr", sizeof(SharedPtr<T>), nullptr);
		type->mIsAssetPtr = true;
		Py_XINCREF(LType::Get<T>()->GetBindingType());
		type->mPyType = LType::Get<T>()->GetBindingType();
		return type;
	}
	static LType* StaticType()
	{
		static LType* type = Init();
		return type;
	}
};

template<typename T>
struct static_type<TAssetPtrArray<T>>
{
	static LType* Init()
	{
		LType* type = NewTemplateType<T>("AssetPtrArray", sizeof(TAssetPtrArray<T>), nullptr);
		type->mIsAssetArray = true;
		Py_XINCREF(&PyList_Type);
		type->mPyType = &PyList_Type;
		return type;
	}

	static LType* StaticType()
	{
		static LType* type = Init();
		return type;
	}
};



}
