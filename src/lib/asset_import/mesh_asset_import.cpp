#define ASSET_IMPORT_EXPORT
#include"mesh_asset_import.h"
#include"render/asset/mesh_asset.h"
namespace luna::assetimport
{
	void LMeshAssetImport::ParsingImportSceneImpl(const resimport::LImportScene& importSceneData, LAssetPack& outAssetPack)
	{
		size_t nodeAllSize = importSceneData.GetNodeSize();
		for (size_t nodeIndex = 0; nodeIndex < nodeAllSize; ++nodeIndex)
		{
			const resimport::LImportSceneNode &nodeValue = importSceneData.GetNode(nodeIndex);
			for (auto& nodeDataValue : nodeValue.mNodeData)
			{
				switch (nodeDataValue.first)
				{
				case resimport::LImportNodeDataType::ImportDataMesh:
				{
					size_t dataIndex = nodeDataValue.second;
					const resimport::LImportNodeDataMesh* dataValue = importSceneData.GetData<resimport::LImportNodeDataMesh>(dataIndex);
					render::MeshAsset* meshValuePtr= outAssetPack.CreateAsset<render::MeshAsset>(nodeValue.mName);
					for (size_t submeshIndex = 0; submeshIndex < dataValue->GetSubMeshSize(); ++submeshIndex)
					{
						render::SubMesh* sub_mesh = TCreateObject<render::SubMesh>();
						const resimport::LImportSubmesh &submeshData = dataValue->GettSubMesh(submeshIndex);
						sub_mesh->mVertexCount = submeshData.mVertexPosition.size();
						sub_mesh->mIndexCount = submeshData.mIndices.size();
						LString subMeshName = submeshData.mName;
						sub_mesh->SetObjectName(subMeshName);
						for(size_t vertexIndex = 0; vertexIndex < sub_mesh->mVertexCount; ++vertexIndex)
						{
							render::BaseVertex newVertexData;
							newVertexData.pos = submeshData.mVertexPosition[vertexIndex];
							newVertexData.normal = submeshData.mVertexNormal[vertexIndex];
							newVertexData.tangent = submeshData.mVertexTangent[vertexIndex];
							newVertexData.color = submeshData.mVertexColor[vertexIndex];
							for (size_t uvChannel = 0; uvChannel < submeshData.mVertexUv[vertexIndex].size(); ++uvChannel)
							{
								newVertexData.uv[uvChannel] = submeshData.mVertexUv[vertexIndex][uvChannel];
							}
							sub_mesh->mVertexData.push_back(newVertexData);
						}
						for (size_t faceIndex = 0; faceIndex < sub_mesh->mIndexCount; ++faceIndex)
						{
							sub_mesh->mIndexData.push_back(submeshData.mIndices[faceIndex]);
						}
						meshValuePtr->mSubMesh.PushBack(sub_mesh);
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}
}