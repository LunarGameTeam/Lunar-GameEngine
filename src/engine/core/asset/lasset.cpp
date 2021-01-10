#include "core/asset/lasset.h"
const LLoadState& LBasicAsset::GetLoadState()
{
	CheckIfLoadingStateChanged(m_object_load_state);
	return m_object_load_state;
}
//默认加载资源
luna::LResult LBasicAsset::InitResource()
{
	luna::LResult check_error;
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


luna::LResult LBasicAsset::InitCommon()
{
	return luna::g_Succeed;
}