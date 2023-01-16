#pragma once

#include "Core/Object/BaseObject.h"
#include "Core/Asset/Asset.h"
#include "Core/Memory/Ptr.h"
#include "Core/Foundation/String.h"
#include "Core/Foundation/Container.h"
#include "Core/Platform/AsyncHandle.h"
#include "Core/Platform/File.h"
#include "Core/Platform/Path.h"
#include "Core/Serialization/JsonDict.h"

#include <json/json.h>

namespace luna
{

//Json Asset
class CORE_API LBinaryWithHeadAsset : public Asset
{
	RegisterTypeEmbedd(LBinaryWithHeadAsset, Asset)
public:
	virtual ~LBinaryWithHeadAsset();
	//Asset资源读入到内存时回调
	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
	//Asset资源写入到磁盘时回调	
	void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;
private:
	virtual void OnAssetBinaryRead(const byte* value) {};
	virtual void OnAssetBinaryWrite(LArray<byte>& data) {};
	template<typename ValueType>
	void BaseValueToByte(const ValueType value, LArray<byte>& data)
	{
		byte memory[16];
		int32_t size_value = sizeof(ValueType) / sizeof(byte);
		memcpy(memory, &value, sizeof(ValueType));
		for (int32_t index = 0; index < size_value; ++index)
		{
			data.push_back(memory[index]);
		}
	};
};


}
