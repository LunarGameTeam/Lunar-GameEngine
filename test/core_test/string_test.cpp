#include "core/core_library.h"
#include "core/file/path.h"
#include "core/file/file_subsystem.h"

using namespace luna;

#include <gtest/gtest.h>

TEST(Core, String)
{
	LString dir = gEngine->GetModule<FileSystem>()->GetPlatformFileManager()->EngineDir();
	LString str = "/log/log.txt";
	LString split = "log";
	LPath path("/log/log.txt");
}

TEST(Core, StringCast)
{
	LString str = ToString(1);
	int i = FromString<int>(str);
	auto f = ToString(1.1f);
}