#include "core/core_module.h"
#include "core/file/file_subsystem.h"

#include "lua_bind/lua_module.h"

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
	gEngine->MainLoop();
	return 1;
}