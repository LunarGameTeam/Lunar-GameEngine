#pragma once

#include "core/object/base_object.h"
#include "core/asset/asset.h"
#include "core/memory/ptr.h"
#include "core/foundation/string.h"
#include "core/foundation/container.h"
#include "core/platform/async_handle.h"
#include "core/platform/file.h"
#include "core/platform/path.h"
#include "core/serialization/dict_data.h"

#include <json/json.h>

namespace luna
{

//Json Asset
class CORE_API JsonAsset : public Asset
{
	RegisterTypeEmbedd(JsonAsset, Asset)
public:
	virtual ~JsonAsset();
	//Asset资源读入到内存时回调
	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;
	//Asset资源写入到磁盘时回调	
	void OnAssetFileWrite(LSharedPtr<Dictionary> meta, LArray<byte>& data) override;

};


}
