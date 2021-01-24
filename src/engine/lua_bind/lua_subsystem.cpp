#include "lua_subsystem.h"
#include "core/config/config.h"
#include "core/misc/path.h"
#include "core/asset/asset_subsystem.h"
#include "lua_script.h"

namespace luna
{

constexpr char SCRIPT_ENTRY[] = "/assets/entry.lua";


bool LuaSubsystem::OnPreInit()
{
	m_default_env = new LuaEnv();
	m_need_tick = true;
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
	AssetSubsystem *asset_sys = gEngine->GetSubsystem<AssetSubsystem>();
	assert(asset_sys->IsInitialized());

	m_default_env->Init();
	LPath entry_file(SCRIPT_ENTRY);

	LSharedPtr<LuaScript> script = asset_sys->LoadAsset<LuaScript>(entry_file);
	assert(script.get() != nullptr);

	m_default_env->GetLuaState().script(script->GetScriptContent().c_str());
	return true;
}

bool LuaSubsystem::OnShutdown()
{
	return true;
}

void LuaSubsystem::Tick()
{
	auto tick = m_default_env->GetLuaState()["tick"];
	if(tick)
		tick();
}


}