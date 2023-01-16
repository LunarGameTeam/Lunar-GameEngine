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
class CORE_API JsonAsset : public Asset
{
	RegisterTypeEmbedd(JsonAsset, Asset)
public:
	virtual ~JsonAsset();
	//Asset资源读入到内存时回调
	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
	//Asset资源写入到磁盘时回调	
	void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;

};


}
