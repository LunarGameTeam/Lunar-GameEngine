#pragma once

#include "core/framework/module.h"
#include "core/file/platform_module.h"
#include "core/file/path.h"
#include "core/memory/ptr.h"
#include "core/memory/garbage_collector.h"
#include "core/reflection/reflection.h"
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
	LBasicAsset* asset;
};

class CORE_API AssetModule : public LModule
{
	RegisterTypeEmbedd(AssetModule, LModule)

public:
	AssetModule();
	virtual ~AssetModule() {};
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

public:
	bool SaveAsset(LBasicAsset *asset, const LPath &path);
	LBasicAsset *LoadAsset(const LPath &path, LType* asset_type);
	LBasicAsset* BindingLoadAsset(const char* path, LType* asset_type);
	template<typename T>
	T* LoadAsset(const LPath &path)
	{
		T* p = dynamic_cast<T*>(LoadAsset(path, LType::Get<T>()));
		return p;
	}

	/// TODO
	template<typename T>
	AssetAsyncHandle<T> LoadAssetAsync(const LPath &path);

	LFileInfo& GetResRoot() { return mRoot; };

	void CollectAssets();
private:
	LFileInfo mRoot;
	LUnorderedMap<LString, AssetCache> mCachedAssets;
};

CORE_API extern AssetModule *sAssetModule;
}
