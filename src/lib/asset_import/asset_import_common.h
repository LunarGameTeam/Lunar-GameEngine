#pragma once
#ifndef ASSET_IMPORT_API
#ifdef ASSET_IMPORT_EXPORT
#define ASSET_IMPORT_API __declspec( dllexport )//∫Í∂®“Â
#else
#define ASSET_IMPORT_API __declspec( dllimport )
#endif
#endif
#include"../import_scene/import_scene.h"
#include"core/asset/asset.h"
#include"core/asset/asset_module.h"
namespace luna::assetimport
{
	class ASSET_IMPORT_API LAssetPack
	{
		LVector<LBasicAsset*> mAssetDatas;
	public:
		template<typename assetType>
		assetType* CreateAsset(LString assetName)
		{
			luna::LType* createType = LType::Get<assetType>();
			LBasicAsset* asset = createType->NewInstance<LBasicAsset>();
			asset->ResetUUID();
			asset->SetObjectName(assetName);
			mAssetDatas.push_back(asset);
			assetType* assetPtr = dynamic_cast<assetType*>(asset);
			return assetPtr;
		}

		void SerializeAllAsset(const LPath& path);

		void SerializeAllAsset(const LPath& path,const LString & assetName);

		LBasicAsset* GetAsset(size_t assetId);
	};

	class ASSET_IMPORT_API LAssetImportBase
	{
	public:
		void ParsingImportScene(const resimport::LImportScene &importSceneData, LAssetPack& outAssetPack);
	private:
		virtual void ParsingImportSceneImpl(const resimport::LImportScene& importSceneData, LAssetPack& outAssetPack) = 0;
	};
}

