#include "Core/Asset/AssetModule.h"
#include "Core/Platform/PlatformModule.h"
#include "Core/Platform/PlatformFile.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Foundation/profile.h"
#include "Core/Foundation/Config.h"

namespace luna
{

RegisterTypeEmbedd_Imp(AssetModule)
{
	cls->Ctor<AssetModule>();
	cls->Binding<Self>();
	cls->BindingMethod<&Self::SaveAsset>("save_asset");
	cls->BindingMethod<&Self::NewAsset>("new_asset");
	cls->BindingMethod<&AssetModule::BindingLoadAsset>("load_asset")
		.Doc("def load_asset(self, asset_name: str, t: Type[T]) -> T:\n\tpass\n");
	BindingModule::Get("luna")->AddType(cls);
	
}


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

Asset* AssetModule::NewAsset(const LString& path, LType* type)
{
	Asset* asset= type->NewInstance<Asset>();
	asset->mAssetPath = path;
	return asset;
}

bool AssetModule::SaveAsset(Asset *asset, const LString &path)
{
	IPlatformFileManager *manager = gEngine->GetTModule<PlatformModule>()->GetPlatformFileManager();
	LArray<byte> data;
	Json::Value val;
	LSharedPtr<JsonDict> meta_data = MakeShared<JsonDict>(val);
	asset->OnAssetFileWrite(meta_data, data);
	manager->WriteSync(path, data);
	return true;
}

SharedPtr<Asset> AssetModule::LoadAsset(const LPath &path, LType *asset_type)
{
	ScopedProfileGuard g(LString::Format("Load {0}", path.AsString()));
	assert(asset_type->IsDerivedFrom(LType::Get<Asset>()));
	static IPlatformFileManager *manager = gEngine->GetTModule<PlatformModule>()->GetPlatformFileManager();

	if (mCachedAssets.find(path.AsString()) != mCachedAssets.end())
	{
		auto sharedPtr = mCachedAssets[path.AsString()]->asset->shared_from_this();
		return std::dynamic_pointer_cast<Asset, SharedObject>(sharedPtr);
	}
	LSharedPtr<Asset> asset(asset_type->NewInstance<Asset>());
	asset->mAssetPath = path.AsString();
	
	
	LString meta_path = path.AsEnginePathString() + ".meta";
	LString meta_str;

	LSharedPtr<LFile> file_data = manager->ReadSync(path);
	Json::Value val;
	LSharedPtr<JsonDict> meta_data = MakeShared<JsonDict>(val);

	if (!file_data->IsOk())
	{		
		return nullptr;
	}

	asset->OnAssetFileRead(meta_data, file_data);
	mCachedAssets[path.AsString()] = new AssetCache();
	mCachedAssets[path.AsString()]->asset = asset.get();
	return asset;
}

void AssetModule::RemoveAsset(Asset* asset)
{
	auto it = mCachedAssets.find(asset->mAssetPath);
	if (it != mCachedAssets.end())
	{
		delete it->second;
		mCachedAssets.erase(it);
	}
}

SharedPtr<Asset> AssetModule::BindingLoadAsset(const char* path, LType* asset_type)
{
	return LoadAsset(path, asset_type);
}

void AssetModule::CollectAssets()
{
	static PlatformModule *file_sys = gEngine->GetTModule<PlatformModule>();
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