#include "lua_subsystem.h"

namespace luna
{

bool LuaSubsystem::OnPreInit()
{
	m_default_env = new LuaEnv();
	return true;
}

bool LuaSubsystem::OnPostInit()
{
	return true;

}

LuaEnv &LuaSubsystem::GetDefaultEnv()
{
	return *m_default_env;
}

bool LuaSubsystem::OnInit()
{
	m_default_env->Init();
	return true;
}

bool LuaSubsystem::OnShutdown()
{
	return true;
}

void LuaSubsystem::Tick()
{
	m_default_env->GetLuaState()["tick"]();
}


}