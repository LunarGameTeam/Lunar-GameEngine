#define ASSET_IMPORT_EXPORT
#include"asset_import_common.h"
#include"render/asset/mesh_asset.h"
namespace luna::assetimport
{
	void LAssetPack::SerializeAllAsset(const LPath& path)
	{
		LString rootPath = path.AsString();
		for (LBasicAsset* assetValue : mAssetDatas)
		{
			LString newPath = rootPath + "/" + assetValue->GetObjectName();
			if (assetValue->GetClass()->GetName() == LType::Get<render::MeshAsset>()->GetName())
			{
				newPath = newPath + ".lmesh";
			}
			sAssetModule->SaveAsset(assetValue, newPath);
		}
	}

	LBasicAsset* LAssetPack::GetAsset(size_t assetId)
	{
		assert(mAssetDatas.size() > assetId);
		return mAssetDatas[assetId];
	};

	void LAssetImportBase::ParsingImportScene(const resimport::LImportScene& importSceneData, LAssetPack& outAssetPack)
	{
		ParsingImportSceneImpl(importSceneData, outAssetPack);
	}
}