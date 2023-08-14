#pragma once
#include "AssetImport/AssetImportCommon.h"
namespace luna::graphics
{
	class MeshAsset;
	class SkeletalMeshAsset;
	class SubMesh;
	class SubMeshSkeletal;
}
namespace luna::asset
{

//场景导出器，该导出器会把场景里所有资源独立导出，不会做合并
class ASSET_IMPORT_API LSceneAssetImport :public LAssetImportBase
{
private:
	void ParsingImportSceneImpl(
		const LString& resSavePath,
		const asset::LImportScene& importSceneData,
		LAssetPack& outAssetPack,
		asset::LSceneAssetNodeMessage* nodeMessage = nullptr
	) override;

	void AddSceneNodeToStaticMesh(const asset::LImportNodeDataMesh* dataValue, graphics::MeshAsset* meshValuePtr);

	void CopySubmeshVertexCommonInfo(const asset::LImportSubmesh& submeshData, graphics::SubMesh* it);
};

}

