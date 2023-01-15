#include "core/core_library.h"
#include "core/platform/path.h"

using namespace luna;

#include <gtest/gtest.h>

TEST(Core, Log)
{
	Log("Core", "format log out {0}", 1.0f);
	LogWarning("Core", "format log out {0}", 10.0f);
	LogError("Core", "format log out {0}", 100.0f);
}