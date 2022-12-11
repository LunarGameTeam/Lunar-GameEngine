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
class CORE_API LJsonAsset : public LBasicAsset
{
	RegisterTypeEmbedd(LJsonAsset, LBasicAsset)
public:
	virtual ~LJsonAsset();
	//Asset资源读入到内存时回调
	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;
	//Asset资源写入到磁盘时回调	
	void OnAssetFileWrite(LSharedPtr<Dictionary> meta, LVector<byte>& data) override;

};


}
