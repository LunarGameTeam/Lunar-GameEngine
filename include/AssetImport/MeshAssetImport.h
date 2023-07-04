#pragma once
#include "AssetImport/AssetImportCommon.h"
namespace luna::render
{
	class MeshAsset;
	class SkeletalMeshAsset;
	class SubMesh;
	class SubMeshSkeletal;
}
namespace luna::asset
{
//经典导出器，该导出器会把场景里的mesh节点按照骨骼归类，并将对应的animation也导出
class ASSET_IMPORT_API LSingleMeshAndAnimationAssetImport :public LAssetImportBase
{
private:
	void ParsingImportSceneImpl(const asset::LImportScene& importSceneData, LAssetPack& outAssetPack) override;

	void AddSceneNodeToStaticMesh(const asset::LImportNodeDataMesh* dataValue, render::MeshAsset* meshValuePtr);

	void AddSceneNodeToSkeletalMesh(const asset::LImportNodeDataMesh* dataValue, render::SkeletalMeshAsset* meshSkeletalValuePtr);

	void CopySubmeshVertexCommonInfo(const asset::LImportSubmesh& submeshData, render::SubMesh* it);

	void CopySubmeshVertexSkinInfo(const asset::LImportSubmesh& submeshData, render::SubMeshSkeletal* it);
};

}

