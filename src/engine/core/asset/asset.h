#pragma once

#include "core/object/object.h"
#include "core/memory/ptr.h"
#include "core/misc/string.h"
#include "core/misc/container.h"
#include "core/file/async_handle.h"
#include "core/file/file.h"
#include "core/file/path.h"
#include "core/io/dict_data.h"

#include <json/json.h>

namespace luna
{

class CORE_API LBasicAsset : public LObject
{
public:
	LBasicAsset(const LPath &asset_path);;
	LBasicAsset();;
	~LBasicAsset();
	//此处接入同步或者异步文件接口
	virtual void OnAssetFileLoad(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file);
	//默认加载资源
	LResult InitResource();
	//检查资源的加载状态
	const LLoadState &GetLoadState();

private:
	virtual void CheckIfLoadingStateChanged(LLoadState &m_object_load_state) ;
	virtual luna::LResult InitCommon();

	LPath m_asset_path;//统一的Asset URL path
	LLoadState m_object_load_state;
};

}
