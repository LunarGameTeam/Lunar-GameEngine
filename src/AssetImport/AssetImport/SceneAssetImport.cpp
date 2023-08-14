#include"Assetimport/SceneAssetImport.h"
#include"Graphics/Asset/MeshAsset.h"
#include"Graphics/Asset/SkeletalMeshAsset.h"
#include"Animation/Asset/SkeletonAsset.h"
#include"Animation/Asset/AnimationClipAsset.h"
namespace luna::asset
{
	void LSceneAssetImport::ParsingImportSceneImpl(
		const LString& resSavePath,
		const LImportScene& importSceneData,
		LAssetPack& outAssetPack,
		asset::LSceneAssetNodeMessage* nodeMessage
	)
	{
		//先挑出所有的模型网格信息
		LArray<size_t> meshDatas = importSceneData.FilterDataByType(LImportNodeDataType::ImportDataMesh);
		LUnorderedMap<size_t, LString> globelMeshIdName;
		for (int32_t meshIndex = 0; meshIndex < meshDatas.size(); ++meshIndex)
		{
			const asset::LImportNodeDataMesh* dataValue = importSceneData.GetData<asset::LImportNodeDataMesh>(meshDatas[meshIndex]);
			LString curMeshAssetName = "emptyNewMesh" + std::to_string(meshIndex);
			graphics::MeshAsset* meshValuePtr = outAssetPack.CreateAsset<graphics::MeshAsset>(curMeshAssetName);
			AddSceneNodeToStaticMesh(dataValue, meshValuePtr);
			globelMeshIdName.insert({ meshDatas[meshIndex] ,resSavePath + "/" + curMeshAssetName + ".lmesh"});
		}
		//暂时先不考虑节点树的父子关系，只挑出有资源的节点，将变换塌陷
		size_t nodeAllSize = importSceneData.GetNodeSize();
		for (size_t nodeIndex = 0; nodeIndex < nodeAllSize; ++nodeIndex)
		{
			const LImportSceneNode &curNode = importSceneData.GetNode(nodeIndex);
			auto meshItor = curNode.mNodeData.find(LImportNodeDataType::ImportDataMesh);
			if (meshItor != curNode.mNodeData.end())
			{
				size_t meshGlobelIndex = meshItor->second;
				nodeMessage->AddNodeToAsset(curNode.mName,globelMeshIdName[meshGlobelIndex], "/assets/built-in/Pbr.mat", curNode.mTranslation, curNode.mRotation, curNode.mScal);
			}
		}
	}

	void LSceneAssetImport::AddSceneNodeToStaticMesh(const asset::LImportNodeDataMesh* dataValue, graphics::MeshAsset* meshValuePtr)
	{
		for (size_t submeshIndex = 0; submeshIndex < dataValue->GetSubMeshSize(); ++submeshIndex)
		{
			graphics::SubMesh* it = TCreateObject<graphics::SubMesh>();
			const asset::LImportSubmesh& submeshData = dataValue->GettSubMesh(submeshIndex);
			CopySubmeshVertexCommonInfo(submeshData, it);
			meshValuePtr->mSubMesh.push_back(it);
		}
	}

	void LSceneAssetImport::CopySubmeshVertexCommonInfo(const asset::LImportSubmesh& submeshData, graphics::SubMesh* it)
	{
		for (size_t vertexIndex = 0; vertexIndex < submeshData.mVertexPosition.size(); ++vertexIndex)
		{
			graphics::BaseVertex newVertexData;
			newVertexData.pos = submeshData.mVertexPosition[vertexIndex];
			newVertexData.normal = submeshData.mVertexNormal[vertexIndex];
			newVertexData.tangent = submeshData.mVertexTangent[vertexIndex];
			newVertexData.color = submeshData.mVertexColor[vertexIndex];
			for (size_t uvChannel = 0; uvChannel < submeshData.mVertexUv[vertexIndex].size(); ++uvChannel)
			{
				newVertexData.uv[uvChannel] = submeshData.mVertexUv[vertexIndex][uvChannel];
			}
			it->mVertexData.push_back(newVertexData);
		}
		for (size_t faceIndex = 0; faceIndex < submeshData.mIndices.size(); ++faceIndex)
		{
			it->mIndexData.push_back(submeshData.mIndices[faceIndex]);
		}
	}
}