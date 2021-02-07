#include "core/asset/asset.h"

namespace luna
{

const LLoadState &LBasicAsset::GetLoadState()
{
	CheckIfLoadingStateChanged(m_object_load_state);
	return m_object_load_state;
}

LBasicAsset::LBasicAsset(const LPath &asset_path) : m_asset_path(asset_path)
{
	m_object_load_state = LLoadState::LOAD_STATE_EMPTY;
}

LBasicAsset::LBasicAsset() : m_asset_path("")
{
	m_object_load_state = LLoadState::LOAD_STATE_EMPTY;
}

LBasicAsset::~LBasicAsset()
{

}

void LBasicAsset::OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file)
{

}

//默认加载资源
LResult LBasicAsset::InitResource()
{
	LResult check_error;
	if (m_object_load_state == LLoadState::LOAD_STATE_EMPTY)
	{
		check_error = InitCommon();
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	return luna::g_Succeed;
}

void LBasicAsset::CheckIfLoadingStateChanged(LLoadState &m_object_load_state)
{

}

LResult LBasicAsset::InitCommon()
{
	return luna::g_Succeed;
}

}