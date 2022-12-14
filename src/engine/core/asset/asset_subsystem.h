#pragma once

#include "core/subsystem/sub_system.h"
#include "core/file/file_subsystem.h"
#include "core/file/path.h"
#include "core/memory/ptr.h"
#include "asset.h"

namespace luna
{

template<typename T>
class AssetAsyncHandle
{

};

struct AssetCache
{
	LSharedPtr<Dictionary> meta;
	LSharedPtr<LBasicAsset> asset;
};

class CORE_API AssetSubsystem : public SubSystem
{

public:
	AssetSubsystem() {};
	virtual ~AssetSubsystem() {};
	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;
	
public:
	template<typename T>
	LSharedPtr<T> LoadAsset(const LPath &path)
	{
		//Cache命中	
		auto &cache = m_cached_assets[path.AsStringAbs()];
		if (cache == nullptr)
		{

		}
		LSharedPtr<Dictionary> meta = cache->meta;
		LSharedPtr<LBasicAsset> asset = cache->asset;
		if (asset.get() != nullptr)
		{
			return std::dynamic_pointer_cast<T>(asset);
		}
		//没有命中
		LSharedPtr<T> t = MakeShared<T>();
		IPlatformFileManager *manager = gEngine->GetSubsystem<FileSubsystem>()->GetPlatformFileManager();
		LString meta_path = path.AsStringAbs() + ".meta";
		LString meta_str;

		if (!manager->ReadStringFromFile(meta_path, meta_str))
			return t;

		LSharedPtr<LFile> file_data = manager->ReadSync(path);
		LSharedPtr<Dictionary> meta_data = MakeShared<Dictionary>();

		if (!file_data->IsOk())
			return t;

		Dictionary::FromBinary(meta_str.c_str(), meta_str.Length(), *meta_data);
			
		//对Asset使用Meta和FileData进行初始化
		t->OnAssetFileLoad(meta_data, file_data);
		m_cached_assets[path.AsStringAbs()]->meta = meta_data;
		m_cached_assets[path.AsStringAbs()]->asset = t;
		return t;
	}

	template<typename T>
	AssetAsyncHandle<T> LoadAssetAsync(const LPath &path);


private:
	LUnorderedMap<LString, AssetCache*> m_cached_assets;
	void CollectAssets();
};
}
