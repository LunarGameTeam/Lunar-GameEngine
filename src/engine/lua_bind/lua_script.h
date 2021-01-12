#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>
#include "core/misc/string.h"
#include "core/asset/lasset.h"

namespace luna
{

class LuaScript : public LBasicAsset
{
public:

	void Init();
	void RunScript(const LString& str);

	sol::state &GetLuaState();

private:
	sol::state m_lua_state;


	void CheckIfLoadingStateChanged(LLoadState &m_object_load_state) override
	{
	}


	luna::LResult InitCommon() override
	{
		return g_Succeed;
	}

};


}