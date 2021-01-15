#include "core/core_module.h"
#include "core/file/file_subsystem.h"
#include "lua_bind/lua_subsystem.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "core/config/config.h"

using namespace luna;

#include <gtest/gtest.h>

void OnFileOpen(LSharedPtr<LFile> file)
{
	return;
}

TEST(LuaEnv, Test0)
{
	LuaSubsystem *lua = gEngine->GetSubsystem<LuaSubsystem>();
	FileSubsystem *file_sys = gEngine->GetSubsystem<FileSubsystem>();
	LPath entry_file("/assets/entry.lua");
	LString content;
	file_sys->GetPlatformFileManager()->ReadStringFromFile(entry_file, content);
	auto f = file_sys->GetPlatformFileManager()->ReadSync(entry_file);
	file_sys->GetPlatformFileManager()->ReadAsync(entry_file, OnFileOpen);

	LuaEnv& env = lua->GetDefaultEnv();
	auto& sta = env.GetLuaState(); 
	sta.script(content.c_str());
}

int main(int argc, const char* argv[])
{
	gEngine = new lunaCore();
	Config<int, 1> test("Lua", "Kay");
	gEngine->RegisterSubsystem<luna::LuaSubsystem>();
	gEngine->RegisterSubsystem<luna::FileSubsystem>();
	gEngine->Run();
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}