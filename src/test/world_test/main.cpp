#include "core/core_module.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include "lua_bind/lua_subsystem.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "core/config/config.h"

using namespace luna;

#include <gtest/gtest.h>



TEST(LuaEnv, Test0)
{
	LuaSubsystem *lua = gEngine->GetSubsystem<LuaSubsystem>();
	FileSubsystem *file_sys = gEngine->GetSubsystem<FileSubsystem>();
	LPath entry_file("/assets/entry.lua");
	LString content;
	file_sys->GetPlatformFileManager()->ReadStringFromFile(entry_file, content);
	LuaEnv& env = lua->GetDefaultEnv();
	auto& sta = env.GetLuaState(); 
	sta.script(content.c_str());
}


int main(int argc, const char* argv[])
{
	gEngine = new lunaCore();
	gEngine->RegisterSubsystem<luna::FileSubsystem>();
	gEngine->RegisterSubsystem<luna::AssetSubsystem>();
	gEngine->RegisterSubsystem<luna::LuaSubsystem>();
	gEngine->Run();
	testing::InitGoogleTest();
	RUN_ALL_TESTS();
	while (gEngine->GetPendingExit())
	{
		gEngine->OnSubsystemFrameBegin(gEngine->GetFrameDelta());
		gEngine->OnSubsystemTick(gEngine->GetFrameDelta());
		gEngine->OnSubsystemFrameEnd(gEngine->GetFrameDelta());
	}
	return 1;
}