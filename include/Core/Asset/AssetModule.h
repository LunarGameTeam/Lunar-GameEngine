#pragma once

#include "Core/Framework/Module.h"
#include "Core/Platform/PlatformModule.h"
#include "Core/Platform/Path.h"
#include "Core/Memory/Ptr.h"
#include "Core/Memory/GarbageCollector.h"
#include "Core/Reflection/Reflection.h"
#include "Asset.h"

namespace luna
{
template<typename T>
class AssetAsyncHandle
{
};

struct AssetCache
{
	LSharedPtr<JsonDict> meta;
	LSharedPtr<Asset> asset;
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
	Asset* NewAsset(const LString& path, LType* type);
	void RemoveAsset(Asset* asset);
	bool SaveAsset(Asset *asset, const LString &path);	
	Asset *LoadAsset(const LPath &path, LType* asset_type);
	
	Asset* BindingLoadAsset(const char* path, LType* asset_type);
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
	LUnorderedMap<LString, AssetCache*> mCachedAssets;
};

CORE_API extern AssetModule *sAssetModule;
}
