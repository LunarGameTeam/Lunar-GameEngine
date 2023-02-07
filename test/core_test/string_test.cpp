#include "Core/CoreMin.h"
#include "Core/Platform/Path.h"
#include "Core/Platform/PlatformModule.h"

using namespace luna;

#include <gtest/gtest.h>

TEST(Core, String)
{
	LString dir = gEngine->GetTModule < PlatformModule > ()->GetPlatformFileManager()->EngineDir();
	LString str = "/Foundation/log.txt";
	LString split = "log";
	LPath path("/Foundation/log.txt");
}

TEST(Core, StringCast)
{
	LString str = ToString(1);
	int i = FromString<int>(str);
	auto f = ToString(1.1f);
}