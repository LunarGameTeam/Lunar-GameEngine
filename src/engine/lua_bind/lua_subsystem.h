#pragma once

#include "core/subsystem/sub_system.h"
#include "core/core_module.h"
#include "lua_env.h"

namespace luna
{

class LUA_BIND_API LuaSubsystem : public SubSystem
{

public:
	LuaSubsystem() {};
	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

	LuaEnv &GetDefaultEnv();

private:
	LuaEnv *m_default_env;
};

}



