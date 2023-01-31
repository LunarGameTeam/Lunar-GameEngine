#include "AssetImport/AssetImportCommon.h"
#include "Graphics/Asset/MeshAsset.h"
namespace luna::asset
{
	void LAssetPack::SerializeAllAsset(const LPath& path)
	{
		LString rootPath = path.AsString();
		for (Asset* assetValue : mAssetDatas)
		{
			LString newPath = rootPath + "/" + assetValue->GetObjectName();
			if (assetValue->GetClass()->GetName() == LType::Get<render::MeshAsset>()->GetName())
			{
				newPath = newPath + ".lmesh";
			}
			sAssetModule->SaveAsset(assetValue, newPath);
		}
	}

	void LAssetPack::SerializeAllAsset(const LPath& path, const LString& assetName)
	{
		LString rootPath = path.AsString();
		for (Asset* assetValue : mAssetDatas)
		{
			LString newPath = rootPath + "/" + assetName;
			if (assetValue->GetClass()->GetName() == LType::Get<render::MeshAsset>()->GetName())
			{
				newPath = newPath + ".lmesh";
			}
			sAssetModule->SaveAsset(assetValue, newPath);
		}
	}

	Asset* LAssetPack::GetAsset(size_t assetId)
	{
		assert(mAssetDatas.size() > assetId);
		return mAssetDatas[assetId];
	};

	void LAssetImportBase::ParsingImportScene(const asset::LImportScene& importSceneData, LAssetPack& outAssetPack)
	{
		ParsingImportSceneImpl(importSceneData, outAssetPack);
	}
}