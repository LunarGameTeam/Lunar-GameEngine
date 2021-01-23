#include "core/core_module.h"
#include "core/file/file_subsystem.h"
#include "lua_bind/lua_subsystem.h"
#include "lua_bind/lua_script.h"
#include "core/asset/asset_subsystem.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "core/config/config.h"

using namespace luna;

#include <gtest/gtest.h>


TEST(LuaEnv, Test0)
{
	LuaSubsystem *lua = gEngine->GetSubsystem<LuaSubsystem>();
	FileSubsystem *file_sys = gEngine->GetSubsystem<FileSubsystem>();
	AssetSubsystem *asset_sys = gEngine->GetSubsystem<AssetSubsystem>();
	LPath entry_file("/assets/entry.lua");
	LSharedPtr<LuaScript> script = asset_sys->LoadAsset<LuaScript>(entry_file);
	

	LuaEnv &env = lua->GetDefaultEnv();
	auto &sta = env.GetLuaState();
	sta.script(script->GetScriptContent().c_str());
	
}

int main(int argc, const char* argv[])
{
	gEngine = new lunaCore();
	gEngine->RegisterSubsystem<luna::LuaSubsystem>();
	gEngine->RegisterSubsystem<luna::FileSubsystem>();
	gEngine->RegisterSubsystem<luna::AssetSubsystem>();
	gEngine->Run();
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
	gEngine->MainLoop();
	return 1;
}