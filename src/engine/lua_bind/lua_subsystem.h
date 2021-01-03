#pragma once

#include "core/subsystem/sub_system.h"
#include "lua_env.h"

namespace luna
{

class LuaSubsystem : public SubSystem
{

public:
	bool OnPreInit() override;


	bool OnPostInit() override;


	bool OnInit() override;


	bool OnShutdown() override;


	void Tick() override;

	LuaEnv &GetDefaultEnv();

private:
	LuaEnv *m_default_env;
};

}



