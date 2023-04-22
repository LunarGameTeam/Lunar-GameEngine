#include "AssetImport/AssetImportCommon.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Animation/Asset/SkeletonAsset.h"
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
			else if (assetValue->GetClass()->GetName() == LType::Get<render::SkeletalMeshAsset>()->GetName())
			{
				newPath = newPath + ".lskelmesh";
			}
			else if (assetValue->GetClass()->GetName() == LType::Get<animation::SkeletonAsset>()->GetName())
			{
				newPath = newPath + ".lskeleton";
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