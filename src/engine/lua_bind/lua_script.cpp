#include "lua_env.h"

void luna::LuaEnv::Init()
{
	m_lua_state.open_libraries(sol::lib::base, sol::lib::package);
}

void luna::LuaEnv::RunScript(const LString &str)
{
}

sol::state &luna::LuaEnv::GetLuaState()
{
	return m_lua_state;
}

