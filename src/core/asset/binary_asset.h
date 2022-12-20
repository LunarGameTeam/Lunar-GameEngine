#pragma once

#include "core/object/base_object.h"
#include "core/asset/asset.h"
#include "core/memory/ptr.h"
#include "core/foundation/string.h"
#include "core/foundation/container.h"
#include "core/file/async_handle.h"
#include "core/file/file.h"
#include "core/file/path.h"
#include "core/serialization/dict_data.h"

#include <json/json.h>

namespace luna
{

//Json Asset
class CORE_API LBinaryWithHeadAsset : public LBasicAsset
{
	RegisterTypeEmbedd(LBinaryWithHeadAsset, LBasicAsset)
public:
	virtual ~LBinaryWithHeadAsset();
	//Asset资源读入到内存时回调
	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;
	//Asset资源写入到磁盘时回调	
	void OnAssetFileWrite(LSharedPtr<Dictionary> meta, LArray<byte>& data) override;
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
