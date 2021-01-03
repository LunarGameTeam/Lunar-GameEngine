#pragma once

#define SOL_ALL_SAFETIES_ON 1

#include <sol/sol.hpp>
#include "core/misc/string.h"

namespace luna
{

class LuaEnv
{
public:

	void Init();
	void RunScript(const LString& str);

	sol::state &GetLuaState();

private:
	sol::state m_lua_state;

};


}