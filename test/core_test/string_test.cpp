#include "core/core_min.h"
#include "core/platform/path.h"
#include "core/platform/platform_module.h"

using namespace luna;

#include <gtest/gtest.h>

TEST(Core, String)
{
	LString dir = gEngine->GetModule < PlatformModule > ()->GetPlatformFileManager()->EngineDir();
	LString str = "/foundation/log.txt";
	LString split = "log";
	LPath path("/foundation/log.txt");
}

TEST(Core, StringCast)
{
	LString str = ToString(1);
	int i = FromString<int>(str);
	auto f = ToString(1.1f);
}