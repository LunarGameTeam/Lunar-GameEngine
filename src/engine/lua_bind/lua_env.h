#pragma once

#include "private_lua.h"
#include "core/misc/string.h"

namespace luna
{

class LUA_BIND_API LuaEnv
{
public:

	void Init();
	void RunScript(const LString& str);

	sol::state &GetLuaState();

private:
	sol::state m_lua_state;
	sol::table m_luna_namespace;
};


}