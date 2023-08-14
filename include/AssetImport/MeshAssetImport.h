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
//���䵼�������õ�������ѳ������mesh�ڵ㰴�չ������࣬������Ӧ��animationҲ����
class ASSET_IMPORT_API LSingleMeshAndAnimationAssetImport :public LAssetImportBase
{
private:
	void ParsingImportSceneImpl(
		const LString& resSavePath, 
		const asset::LImportScene& importSceneData,
		LAssetPack& outAssetPack,
		asset::LSceneAssetNodeMessage* nodeMessage = nullptr
	) override;

	void AddSceneNodeToStaticMesh(const asset::LImportNodeDataMesh* dataValue, graphics::MeshAsset* meshValuePtr);

	void AddSceneNodeToSkeletalMesh(const asset::LImportNodeDataMesh* dataValue, graphics::SkeletalMeshAsset* meshSkeletalValuePtr);

	void CopySubmeshVertexCommonInfo(const asset::LImportSubmesh& submeshData, graphics::SubMesh* it);

	void CopySubmeshVertexSkinInfo(const asset::LImportSubmesh& submeshData, graphics::SubMeshSkeletal* it);
};

}

