#include "core/core_module.h"
#include "core/file/path.h"
#include "core/file/file_subsystem.h"
#include "core/asset/asset_subsystem.h"
#include "core/asset/asset.h"

using namespace luna;

#include <gtest/gtest.h>

void OnFileOpen(LSharedPtr<FileAsyncHandle> file)
{
	return;
}

TEST(Core, File)
{
	FileSubsystem *file_sys = gEngine->GetSubsystem<FileSubsystem>();	
	AssetSubsystem *asset_sys = gEngine->GetSubsystem<AssetSubsystem>();
	IPlatformFileManager *manager = file_sys->GetPlatformFileManager();

	LFileInfo info;
	manager->GetFileInfo("/assets", info, true);

	LFileInfo assets_dir;
	manager->GetFileInfoRecursive("/assets", assets_dir);
	bool ignore_exists = true;
	for (auto &file : assets_dir.folder_contents)
	{
		auto &info = file.second;
		if (!info.path.EndsWith(".meta"))
		{
			LString meta_path = info.path + ".meta";
			//不存在meta生成			
			if (ignore_exists || assets_dir.folder_contents.find(meta_path) == assets_dir.folder_contents.end())
			{				
				LString content = "{\n}";
				manager->WriteStringToFile(meta_path, content);
			}
			LString file_name = info.path.Substr(0, info.path.FindLast('.'));
		}
	}

	LPath entry_file("/assets/entry.lua");
	manager->ReadAsync(entry_file, OnFileOpen);
	asset_sys->LoadAsset<LBasicAsset>("/assets/entry.lua");
}