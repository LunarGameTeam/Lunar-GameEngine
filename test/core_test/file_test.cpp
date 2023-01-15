#include "core/core_library.h"
#include "core/platform/path.h"
#include "core/platform/platform_module.h"
#include "core/asset/asset_module.h"
#include "core/asset/asset.h"

using namespace luna;

#include <gtest/gtest.h>

void OnFileOpen(LSharedPtr<FileAsyncHandle> file)
{
	return;
}

TEST(Core, File)
{
	PlatformModule *file_sys = gEngine->GetModule<PlatformModule>();
	AssetModule*asset_sys = gEngine->GetModule<AssetModule>();
	IPlatformFileManager *manager = file_sys->GetPlatformFileManager();

	//获取一个文件/文件夹的信息
	LFileInfo info;
	manager->GetFileInfo("/assets", info, true);
	//递归获取一个文件/文件夹的信息
	LFileInfo assets_dir;
	manager->GetFileInfoRecursive("/assets", assets_dir);
	//异步读
	LPath entry_file("/assets/entry.lua");
	manager->ReadAsync(entry_file, OnFileOpen);
}