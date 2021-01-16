#include "core/core_module.h"
#include "core/file/path.h"
#include "core/file/file_subsystem.h"

using namespace luna;

#include <gtest/gtest.h>

TEST(Core, Log)
{
	LogVerboseFormat(E_Core, "format log out %f", 1.0f);
	LogWarningFormat(E_Core, g_Succeed, "format log out %f", 10.0f);
	LogErrorFormat(E_Core, g_Failed, "format log out %f", 100.0f);
}