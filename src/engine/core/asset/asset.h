#pragma once

#include "core/object/object.h"
#include "core/file/path.h"
#include "core/misc/string.h"
#include "core/misc/container.h"
#include "core/file/async_handle.h"
#include "core/file/file.h"
#include <json/json.h>

namespace luna
{

class CORE_API AssetMetaData
{
public:
	Json::Value m_value;
};


class CORE_API LBasicAsset : public LObject
{
public:
	LBasicAsset(const LPath &asset_path)
		: m_asset_path(asset_path)
	{
		m_object_load_state = LLoadState::LOAD_STATE_EMPTY;
	};
	LBasicAsset()
		:m_asset_path("")
	{
		m_object_load_state = LLoadState::LOAD_STATE_EMPTY;
	};
	~LBasicAsset();

	//此处接入同步或者异步文件接口
	virtual void OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file)
	{

	}

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
