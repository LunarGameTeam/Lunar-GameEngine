#pragma once
#include "fbx_mesh_importer.h"
namespace luna::lfbx
{
	void GetHashVertex(
		VertexDataFullMember vertData,
		LString & hashString
	)
	{
		for (int32_t i = 0; i < 4; ++i)
		{
			const int64_t hash_num = vertData.mPosition.mData[i] / deltaEpsion;
			hashString.Append(std::to_string(hash_num).c_str());
		}

		for (int32_t i = 0; i < 4; ++i)
		{
			const int64_t hash_num = vertData.mNormal.mData[i] / deltaEpsion;
			hashString.Append(std::to_string(hash_num).c_str());
		}

		for (int32_t i = 0; i < 4; ++i)
		{
			const int64_t hash_num = vertData.mTangent.mData[i] / deltaEpsion;
			hashString.Append(std::to_string(hash_num).c_str());
		}

		{
			int64_t hash_num = vertData.mColor.mRed / deltaEpsion;
			hashString.Append(std::to_string(hash_num).c_str());
			hash_num = vertData.mColor.mGreen / deltaEpsion;
			hashString.Append(std::to_string(hash_num).c_str());
			hash_num = vertData.mColor.mBlue / deltaEpsion;
			hashString.Append(std::to_string(hash_num).c_str());
			hash_num = vertData.mColor.mAlpha / deltaEpsion;
			hashString.Append(std::to_string(hash_num).c_str());
		}
		
		for (int32_t uv_channel = 0; uv_channel < vertData.mUvs.size(); ++uv_channel)
		{
			for (int32_t i = 0; i < 2; ++i)
			{
				const int64_t hash_num = vertData.mUvs[uv_channel].mData[i] / deltaEpsion;
				hashString.Append(std::to_string(hash_num).c_str());
			}
		}

	}

	void LFbxImporterMesh::ParsingDataImpl(const LFbxDataBase* fbxDataInput, const LFbxNodeBase& fbxNodeInput, ImportData::LImportScene& outputScene)
	{
		const LFbxDataMesh* meshData = static_cast<const LFbxDataMesh*>(fbxDataInput);
		size_t newDataIndex = outputScene.AddNewData<ImportData::LImportNodeDataMesh>();
		ImportData::LImportNodeDataMesh* nowOutData = outputScene.GetData<ImportData::LImportNodeDataMesh>(newDataIndex);
		LVector<VertexDataFullCombine> newCombineData;
		newCombineData.resize(meshData->GetMaterialCount());
		CombineVertexData(meshData, newCombineData);
		LVector<VertexDataFullCombine> newOptimizeData;
		LVector<LVector<int32_t>> newOptimizeIndex;
		OptimizeCombineVert(newCombineData, newOptimizeData, newOptimizeIndex);
		for (int32_t submeshId = 0; submeshId < newOptimizeData.size(); ++submeshId)
		{
			//��ʱ��Ϊsubmeshid���ǲ���id
			size_t realSubIndex = nowOutData->AddSubMeshMessage(fbxNodeInput.mName, submeshId);
			VertexDataFullCombine &newOptimizeDataMember = newOptimizeData[submeshId];
			LVector<int32_t>& newOptimizeIndexMember = newOptimizeIndex[submeshId];
			for (int32_t vertId = 0; vertId < newOptimizeDataMember.mVertexs.size(); ++vertId)
			{
				VertexDataFullMember& newVertData = newOptimizeDataMember.mVertexs[vertId];
				LVector3f vertPosition, vertNormal;
				LVector4f vertTangent, vertColor;
				FbxVector4ToVector3(newVertData.mPosition, vertPosition);
				FbxVector4ToVector3(newVertData.mNormal, vertNormal);
				FbxVector4ToVector4(newVertData.mTangent, vertTangent);
				FbxColorToVector4(newVertData.mColor, vertColor);
				LVector<LVector2f> vertUv;
				vertUv.resize(newVertData.mUvs.size());
				for (size_t uvChannel = 0; uvChannel < newVertData.mUvs.size(); ++uvChannel)
				{
					FbxVector2ToVector2(newVertData.mUvs[uvChannel],vertUv[uvChannel]);
				}
				nowOutData->AddFullVertex(realSubIndex,vertPosition, vertNormal, vertTangent, vertUv, vertColor);
			}
			for (int32_t vertId = 0; vertId < newOptimizeIndexMember.size(); ++vertId)
			{
				nowOutData->AddFaceIndexDataToSubmesh(realSubIndex, newOptimizeIndexMember[vertId]);
			}
		}
	}

	void LFbxImporterMesh::CombineVertexData(const LFbxDataMesh* meshData, LVector<VertexDataFullCombine>& vertexCombineData)
	{
		//��һ���Ȱ����е�fbx layer�������ݵ�������������
		for (int i = 0; i < meshData->GetFaceSize(); ++i)
		{
			const FbxFaceData& faceData = meshData->GetFaceDataByIndex(i);
			for (int j = 0; j < 3; j++)
			{
				int32_t vertIndex = faceData.mIndex[j];
				size_t polygenIndex = i * 3 + j;
				VertexDataFullMember new_vertex;
				new_vertex.mPosition = meshData->GetControlPointByIndex(vertIndex);
				new_vertex.mNormal = meshData->GetNormalByIndex(vertIndex, polygenIndex);
				new_vertex.mTangent = meshData->GetTangentByIndex(vertIndex, polygenIndex);
				new_vertex.mColor = meshData->GetColorByIndex(vertIndex, polygenIndex);
				new_vertex.mUvs.resize(meshData->GetUvChannelSize());
				for (int32_t uv_channel = 0; uv_channel < meshData->GetUvChannelSize(); ++uv_channel)
				{
					new_vertex.mUvs[uv_channel] = meshData->GetUvByIndex(vertIndex, faceData.mTextureUvIndex[j], uv_channel);
				}
				new_vertex.baseIndex = polygenIndex;
				vertexCombineData[faceData.mMaterialIndex].mVertexs.push_back(new_vertex);
			}
		}
	}
	
	void LFbxImporterMesh::OptimizeCombineVert(
		const LVector<VertexDataFullCombine>& vertexCombineDataIn,
		LVector<VertexDataFullCombine>& vertexCombineDataOut,
		LVector<LVector<int32_t>> &OptimizeIndex
	)
	{
		//��һ�����ж����Ż����Ѷ���������ͬ�Ķ�����кϲ�������¼�¶��������
		vertexCombineDataOut.resize(vertexCombineDataIn.size());
		OptimizeIndex.resize(vertexCombineDataIn.size());
		for (size_t submesshId = 0; submesshId < vertexCombineDataIn.size(); ++submesshId)
		{
			const VertexDataFullCombine& vertexCombineDataInMember = vertexCombineDataIn[submesshId];
			VertexDataFullCombine& vertexCombineDataOutMember = vertexCombineDataOut[submesshId];
			LVector<int32_t>& OptimizeIndexMember = OptimizeIndex[submesshId];

			LUnorderedMap<LString, VertexDataFullMember> repeatCheckMap;
			LUnorderedMap<size_t, size_t> vertIndexMap;
			size_t vertSize = vertexCombineDataInMember.mVertexs.size();
			OptimizeIndexMember.resize(vertSize);
			for (int32_t vertIndex = 0; vertIndex < vertSize; ++vertIndex)
			{
				LString hashData;
				GetHashVertex(
					vertexCombineDataInMember.mVertexs[vertIndex],
					hashData
				);
				auto repeatFindData = repeatCheckMap.find(hashData);
				if (repeatFindData == repeatCheckMap.end())
				{
					size_t vertIndexNew = vertexCombineDataOutMember.mVertexs.size();
					vertexCombineDataOutMember.mVertexs.push_back(vertexCombineDataInMember.mVertexs[vertIndex]);
					repeatCheckMap.insert(std::pair<LString, VertexDataFullMember>(hashData, vertexCombineDataInMember.mVertexs[vertIndex]));
					OptimizeIndexMember[vertIndex] = vertIndexNew;
				}
				else
				{
					OptimizeIndexMember[vertIndex] = OptimizeIndexMember[repeatFindData->second.baseIndex];
				}
			}
		}
		
	}

}