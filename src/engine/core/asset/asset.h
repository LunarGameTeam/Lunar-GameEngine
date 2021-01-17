#pragma once

#include "core/object/object.h"
#include "core/file/path.h"
#include "core/misc/string.h"
#include "core/misc/container.h"
#include "core/file/async_handle.h"
#include "core/file/file.h"

namespace luna
{

class LBasicAsset : public LObject
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
	void OnAssetFileLoad(LSharedPtr<LFile> file);

	//默认加载资源
	LResult InitResource();
	//检查资源的加载状态
	const LLoadState &GetLoadState();

private:
	virtual void CheckIfLoadingStateChanged(LLoadState &m_object_load_state) = 0;
	virtual luna::LResult InitCommon() = 0;

	LPath m_asset_path;//统一的Asset URL path
	LLoadState m_object_load_state;
};

}
