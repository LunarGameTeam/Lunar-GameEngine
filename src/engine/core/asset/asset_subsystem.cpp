#include "asset_subsystem.h"
#include "core/engine/engine.h"
#include "core/file/file_subsystem.h"
#include "core/file/platform_file.h"
#include "core/misc/profile.h"
#include "core/config/config.h"


namespace luna
{

Config<int, 0> g_create_meta("Asset", "IsAutoCreateAssetMeta");

bool AssetSubsystem::OnPreInit()
{
	return true;
}

bool AssetSubsystem::OnPostInit()
{
	return true;
}

bool AssetSubsystem::OnInit()
{
	CollectAssets();
	return true;
}

bool AssetSubsystem::OnShutdown()
{
	return true;
}

void AssetSubsystem::Tick(float delta_time)
{

}

void AssetSubsystem::CollectAssets()
{

	static FileSubsystem *file_sys = gEngine->GetSubsystem<FileSubsystem>();
	assert(file_sys && file_sys->IsInitialized());
	static IPlatformFileManager *file_manager = file_sys->GetPlatformFileManager();
	LPath asset_dir_path = "/assets";
	LFileInfo asset_dir;
	if (!file_manager->GetFileInfoRecursive(asset_dir_path, asset_dir, true))
		LogError(E_Core, g_Failed, "Assets folder not exists");
	ProfileGuard g;
	for (auto &file : asset_dir.folder_contents)
	{
		auto &info = file.second;
		if (!info.path.EndsWith(".meta"))
		{
			LString meta_path = info.path + ".meta";
			//不存在meta生成			
			if (asset_dir.folder_contents.find(meta_path) == asset_dir.folder_contents.end())
			{
				LString content = "{\n}";
				file_manager->WriteStringToFile(meta_path, content);
			}
			LString file_name = info.path.Substr(0, info.path.FindLast('.'));
			m_cached_assets[info.path] = new AssetCache();
		}
	}	
}

}
