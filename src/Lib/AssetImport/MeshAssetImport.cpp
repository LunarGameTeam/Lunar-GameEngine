#include"Lib/Assetimport/MeshAssetImport.h"
#include"Graphics/Asset/MeshAsset.h"
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
						render::SubMesh* it = TCreateObject<render::SubMesh>();
						const resimport::LImportSubmesh& submeshData = dataValue->GettSubMesh(submeshIndex);
						LString subMeshName = submeshData.mName;
						it->SetObjectName(subMeshName);
						for(size_t vertexIndex = 0; vertexIndex < submeshData.mVertexPosition.size(); ++vertexIndex)
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
							it->mVertexData.push_back(newVertexData);
						}
						for (size_t faceIndex = 0; faceIndex < submeshData.mIndices.size(); ++faceIndex)
						{
							it->mIndexData.push_back(submeshData.mIndices[faceIndex]);
						}
						meshValuePtr->mSubMesh.PushBack(it);
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