#include "core/core_module.h"
#include "core/file/path.h"
#include "core/file/file_subsystem.h"

using namespace luna;

#include <gtest/gtest.h>

void OnFileOpen(LSharedPtr<FileAsyncHandle> file)
{
	return;
}

TEST(Core, File)
{
	FileSubsystem *file_sys = gEngine->GetSubsystem<FileSubsystem>();
	IPlatformFileManager *manager = file_sys->GetPlatformFileManager();
	LPath entry_file("/assets/entry.lua");
	manager->ReadAsync(entry_file, OnFileOpen);
}