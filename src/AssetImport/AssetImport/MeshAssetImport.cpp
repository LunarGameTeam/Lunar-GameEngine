#include"Assetimport/MeshAssetImport.h"
#include"Graphics/Asset/MeshAsset.h"
#include"Graphics/Asset/SkeletalMeshAsset.h"
#include"Animation/Asset/SkeletonAsset.h"
namespace luna::asset
{
	void LMeshAssetImport::ParsingImportSceneImpl(const asset::LImportScene& importSceneData, LAssetPack& outAssetPack)
	{
		//mesh 导出器会把所有的mesh node进行合并，因此只需要创建两个总的mesh asset
		render::MeshAsset* meshValuePtr = nullptr;
		render::SkeletalMeshAsset* meshSkeletalValuePtr = nullptr;
		LUnorderedSet<size_t> existSkeleton;
		size_t nodeAllSize = importSceneData.GetNodeSize();
		for (size_t nodeIndex = 0; nodeIndex < nodeAllSize; ++nodeIndex)
		{
			const asset::LImportSceneNode &nodeValue = importSceneData.GetNode(nodeIndex);
			for (auto& nodeDataValue : nodeValue.mNodeData)
			{
				switch (nodeDataValue.first)
				{
				case asset::LImportNodeDataType::ImportDataMesh:
				{
					size_t dataIndex = nodeDataValue.second;
					const asset::LImportNodeDataMesh* dataValue = importSceneData.GetData<asset::LImportNodeDataMesh>(dataIndex);
					if (dataValue->HasSkin())
					{
						if (meshSkeletalValuePtr == nullptr)
						{
							meshSkeletalValuePtr = outAssetPack.CreateAsset<render::SkeletalMeshAsset>("emptyNewSkeletonMesh");
						}
						AddSceneNodeToSkeletalMesh(dataValue, meshSkeletalValuePtr);
					}
					else
					{
						if (meshValuePtr == nullptr)
						{
							meshValuePtr = outAssetPack.CreateAsset<render::MeshAsset>("emptyNewMesh");
						}
						AddSceneNodeToStaticMesh(dataValue, meshValuePtr);
					}
					break;
				}
				case asset::LImportNodeDataType::ImportDataSkeleton:
				{
					size_t dataIndex = nodeDataValue.second;
					if (existSkeleton.find(dataIndex) != existSkeleton.end())
					{
						continue;
					}
					const asset::LImportNodeDataSkeleton* dataValue = importSceneData.GetData<asset::LImportNodeDataSkeleton>(dataIndex);
					animation::SkeletonAsset* meshValuePtr = outAssetPack.CreateAsset<animation::SkeletonAsset>("emptyNewMesh");
					
					const LArray<SkeletonBoneData>& skeletonBonesData = dataValue->GetBones();
					for (size_t boneIndex = 0; boneIndex < skeletonBonesData.size();  ++boneIndex)
					{
						animation::LSingleBone newBoneData;
						const SkeletonBoneData& importBoneData = skeletonBonesData[boneIndex];
						newBoneData.mBoneName = importBoneData.mBoneName;
						newBoneData.mBaseTranslation = importBoneData.mInitLocation;
						newBoneData.mBaseRotation = importBoneData.mInitRotation;
						newBoneData.mBaseScal = importBoneData.mInitScal;
						newBoneData.mParentIndex = importBoneData.mParentId;
						meshValuePtr->AddBoneToTree(newBoneData);
					}
					existSkeleton.insert(dataIndex);
				}
				default:
					break;
				}
			}
		}
	}

	void LMeshAssetImport::AddSceneNodeToStaticMesh(const asset::LImportNodeDataMesh* dataValue, render::MeshAsset* meshValuePtr)
	{
		for (size_t submeshIndex = 0; submeshIndex < dataValue->GetSubMeshSize(); ++submeshIndex)
		{
			render::SubMesh* it = TCreateObject<render::SubMesh>();
			const asset::LImportSubmesh& submeshData = dataValue->GettSubMesh(submeshIndex);
			CopySubmeshVertexCommonInfo(submeshData,it);
			meshValuePtr->mSubMesh.push_back(it);
		}
	}

	void LMeshAssetImport::AddSceneNodeToSkeletalMesh(const asset::LImportNodeDataMesh* dataValue, render::SkeletalMeshAsset* meshSkeletalValuePtr)
	{
		for (size_t submeshIndex = 0; submeshIndex < dataValue->GetSubMeshSize(); ++submeshIndex)
		{
			render::SubMeshSkeletal* it = TCreateObject<render::SubMeshSkeletal>();
			const asset::LImportSubmesh& submeshData = dataValue->GettSubMesh(submeshIndex);
			CopySubmeshVertexCommonInfo(submeshData, it);
			CopySubmeshVertexSkinInfo(submeshData, it);
			it->mRefBonePose = submeshData.mRefBonePose;
			it->mRefBoneName = submeshData.mRefBoneName;
			meshSkeletalValuePtr->mSubMesh.push_back(it);
		}
	}

	void LMeshAssetImport::CopySubmeshVertexCommonInfo(const asset::LImportSubmesh& submeshData,render::SubMesh* it)
	{
		for (size_t vertexIndex = 0; vertexIndex < submeshData.mVertexPosition.size(); ++vertexIndex)
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
	}

	void LMeshAssetImport::CopySubmeshVertexSkinInfo(const asset::LImportSubmesh& submeshData, render::SubMeshSkeletal* it)
	{
		render::SubMeshSkeletal* skeletonPointer = static_cast<render::SubMeshSkeletal*>(it);
		for (size_t vertexIndex = 0; vertexIndex < submeshData.mVertexPosition.size(); ++vertexIndex)
		{
			render::SkinVertex newSkinData;
			for (size_t refId = 0; refId < asset::gSkinPerVertex; refId += 2)
			{
				//将两个骨骼ID合并为16位保存
				uint32_t ref1Value = submeshData.mRefBone[vertexIndex * asset::gSkinPerVertex + refId];
				uint32_t ref2Value = submeshData.mRefBone[vertexIndex * asset::gSkinPerVertex + refId + 1];
				uint16_t ref1ValueBit = static_cast<uint16_t>(ref1Value);
				uint16_t ref2ValueBit = static_cast<uint16_t>(ref2Value);
				uint32_t CombineRefValueBit = LMath::CombineUint16(ref1ValueBit, ref2ValueBit);
				newSkinData.mRefBone[refId / 2] = CombineRefValueBit;
				//将两个骨骼权重合并为16位保存
				float weight1Value = submeshData.mWeight[vertexIndex * asset::gSkinPerVertex + refId];
				float weight2Value = submeshData.mWeight[vertexIndex * asset::gSkinPerVertex + refId + 1];
				uint16_t weight1ValueBit = LMath::NormalizedFloatToUnit16(weight1Value);
				uint16_t weight2ValueBit = LMath::NormalizedFloatToUnit16(weight2Value);
				uint32_t CombineWeightValueBit = LMath::CombineUint16(ref1ValueBit, ref2ValueBit);
				newSkinData.mWeight[refId / 2] = CombineWeightValueBit;
			}
			skeletonPointer->mSkinData.push_back(newSkinData);
		}
		skeletonPointer->mRefBoneName = submeshData.mRefBoneName;
		skeletonPointer->mRefBonePose = submeshData.mRefBonePose;
	}
}