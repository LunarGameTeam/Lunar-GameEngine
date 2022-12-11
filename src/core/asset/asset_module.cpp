#include "asset_module.h"
#include "core/file/platform_module.h"
#include "core/file/platform_file.h"
#include "core/framework/luna_core.h"
#include "core/foundation/profile.h"
#include "core/foundation/config.h"

namespace luna
{

RegisterTypeEmbedd_Imp(AssetModule)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
	cls->BindingMethod<&AssetModule::BindingLoadAsset>("load_asset")
		.Doc("def load_asset(self, asset_name: str, t: Type[T]) -> T:\n\tpass\n");
}


Config<int> g_create_meta("Asset", "IsAutoCreateAssetMeta", 0);

CORE_API AssetModule *sAssetModule = nullptr;

AssetModule::AssetModule()
{
	assert(sAssetModule == nullptr);
	sAssetModule = this;
}

bool AssetModule::OnLoad()
{
	return true;
}

bool AssetModule::OnInit()
{
	CollectAssets();
	return true;
}

bool AssetModule::OnShutdown()
{
	mCachedAssets.clear();
	return true;
}

void AssetModule::Tick(float delta_time)
{
}

bool AssetModule::SaveAsset(LBasicAsset *asset, const LPath &path)
{
	IPlatformFileManager *manager = gEngine->GetModule<PlatformModule>()->GetPlatformFileManager();
	LVector<byte> data;
	Json::Value val;
	LSharedPtr<Dictionary> meta_data = MakeShared<Dictionary>(val);
	asset->OnAssetFileWrite(meta_data, data);
	manager->WriteSync(path, data);
	return true;
}

LBasicAsset *AssetModule::LoadAsset(const LPath &path, LType *asset_type)
{
	ScopedProfileGuard g(LString::Format("Load {0}", path.AsString()));
	assert(asset_type->IsDerivedFrom(LType::Get<LBasicAsset>()));
	static IPlatformFileManager *manager = gEngine->GetModule<PlatformModule>()->GetPlatformFileManager();

	if (mCachedAssets.find(path.AsString()) != mCachedAssets.end())
	{
		return mCachedAssets[path.AsString()].asset;
	}
	LBasicAsset *asset = asset_type->NewInstance<LBasicAsset>();
	//去掉
	asset->mUUID = boost::uuids::nil_uuid();
	asset->mAssetPath = path;
	
	
	LString meta_path = path.AsStringAbs() + ".meta";
	LString meta_str;

	LSharedPtr<LFile> file_data = manager->ReadSync(path);
	Json::Value val;
	LSharedPtr<Dictionary> meta_data = MakeShared<Dictionary>(val);

	if (!file_data->IsOk())
	{
		delete asset;
		return nullptr;
	}

	auto method = asset_type->GetMethod("on_asset_file_read");
	method.InvokeMember<void>(asset, meta_data, file_data);
	mCachedAssets[path.AsString()].asset = asset;
	return asset;
}

LBasicAsset* AssetModule::BindingLoadAsset(const char* path, LType* asset_type)
{
	return LoadAsset(path, asset_type);
}

void AssetModule::CollectAssets()
{
	static PlatformModule *file_sys = gEngine->GetModule<PlatformModule>();
	assert(file_sys);
	static IPlatformFileManager *file_manager = file_sys->GetPlatformFileManager();
	LPath asset_dir_path = "/assets";
	if (!file_manager->GetFileInfo(asset_dir_path, mRoot, true))
		LogError("Core", "Assets folder not exists");
 	return;
// 	ScopedProfileGuard g;
// 	for (auto &file : asset_dir.folder_contents)
// 	{
// 		auto &info = file.second;
// 		if (!info.path.EndsWith(".meta"))
// 		{
// 			LString meta_path = info.path + ".meta";
// 			//不存在meta生成			^
// 			if (asset_dir.folder_contents.find(meta_path) == asset_dir.folder_contents.end())
// 			{
// 				LString content = "{\n}";
// 				file_manager->WriteStringToFile(meta_path, content);
// 			}
// 			LString file_name = info.path.Substr(0, info.path.FindLast('.'));			
// 		}
// 	}
}
}