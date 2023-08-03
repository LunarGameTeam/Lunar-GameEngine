#pragma once
#include "AssetImport/GltfParser/GltfImporter/GltfMeshImporter.h"
#include "AssetImport/GltfParser/GltfLoader/GltfMeshLoader.h"

namespace luna::asset
{
	void LGltfContextComponentMesh::Insert(const LString& meshHash, size_t meshDataId)
	{
		mExistMeshMap.insert({ meshHash ,meshDataId });
	}

	bool LGltfContextComponentMesh::Check(const LString& meshHash)
	{
		if (mExistMeshMap.find(meshHash) != mExistMeshMap.end())
		{
			return true;
		}
		return false;
	}

	size_t LGltfContextComponentMesh::Get(const LString& meshHash)
	{
		auto data = mExistMeshMap.find(meshHash);
		if (data != mExistMeshMap.end())
		{
			return data->second;
		}
		return (size_t)-1;
	}
	static const float gltfMeshDeltaEpsion = 1.0e-4;
	void LGltfImporterMesh::AppendVector2ToString(const LVector2f& vecData, LString& hashString)
	{
		int64_t hash_num = vecData.x() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
		hash_num = vecData.y() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
	}

	void LGltfImporterMesh::AppendVector3ToString(const LVector3f &vecData, LString &hashString)
	{
		int64_t hash_num = vecData.x() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
		hash_num = vecData.y() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
		hash_num = vecData.z() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
	}

	void LGltfImporterMesh::AppendVector4ToString(const LVector4f& vecData, LString& hashString)
	{
		int64_t hash_num = vecData.x() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
		hash_num = vecData.y() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
		hash_num = vecData.z() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
		hash_num = vecData.w() / gltfMeshDeltaEpsion;
		hashString.Append(std::to_string(hash_num).c_str());
	}

	LString LGltfImporterMesh::GenerateMeshHash(const LGltfDataMesh* gltfMeshData)
	{
		LString hashString;
		size_t subMeshSize = gltfMeshData->GetSubMeshSize();
		hashString.Append(std::to_string(subMeshSize).c_str());
		for (size_t subMeshIndex = 0; subMeshIndex < subMeshSize; ++subMeshIndex)
		{
			const GltfSubmeshData& subMeshData = gltfMeshData->GetSubMesh(subMeshIndex);
			size_t vertexSize = subMeshData.mPosition.size() / 3;
			hashString.Append(std::to_string(vertexSize).c_str());
			for (size_t vertexIndex = 0; vertexIndex < vertexSize; ++vertexIndex)
			{
				//position
				LVector3f positionData;
				memcpy(positionData.data(), &subMeshData.mPosition.data()[vertexIndex * 3], 3 * sizeof(float));
				AppendVector3ToString(positionData, hashString);
				//normal
				LVector3f normalData;
				normalData.setZero();
				if (subMeshData.mNormal.size() > 0)
				{
					memcpy(normalData.data(), &subMeshData.mNormal.data()[vertexIndex * 3], 3 * sizeof(float));
					AppendVector3ToString(normalData, hashString);
				}
				//tangent
				LVector4f tangentData;
				tangentData.setZero();
				if (subMeshData.mTangent.size() > 0)
				{
					memcpy(tangentData.data(), &subMeshData.mTangent.data()[vertexIndex * 4], 4 * sizeof(float));
					AppendVector4ToString(tangentData, hashString);
				}
				//color
				if (subMeshData.mColor.size() > 0)
				{
					uint32_t color = subMeshData.mColor[vertexIndex];
					hashString.Append(std::to_string(color).c_str());
				}
				//uv
				if (subMeshData.mUv0.size() > 0)
				{
					LVector2f uv0Data;
					memcpy(uv0Data.data(), &subMeshData.mUv0.data()[vertexIndex * 2], 2 * sizeof(float));
					AppendVector2ToString(uv0Data, hashString);
				}
				if (subMeshData.mUv1.size() > 0)
				{
					LVector2f uv1Data;
					memcpy(uv1Data.data(), &subMeshData.mUv1.data()[vertexIndex * 2], 2 * sizeof(float));
					AppendVector2ToString(uv1Data, hashString);
				}
			}
			size_t faceIndexSize = subMeshData.mFaceIndex.size();
			hashString.Append(std::to_string(faceIndexSize).c_str());
			for (size_t faceIndex = 0; faceIndex < faceIndexSize; ++faceIndex)
			{
				hashString.Append(std::to_string(subMeshData.mFaceIndex[faceIndex]).c_str());
			}
		}

		size_t stringLength = hashString.Length();
		LString stringSample = "";
		if (hashString.Length() > 15)
		{
			stringSample = hashString.Substr(hashString.Length() / 2, 5);
		}
		else
		{
			stringSample = hashString;
		}
		LString hashStringBase = std::to_string(hashString.Hash());
		LString hashResult = hashStringBase;
		hashResult.Append(std::to_string(stringLength).c_str());
		hashResult.Append(stringSample);
		return hashResult;
	}

	void LGltfImporterMesh::ParsingDataImpl(
		const LGltfDataBase* gltfDataInput,
		const LArray<size_t>& gltfNodesInput,
		LGltfImportContext& dataContext,
		LImportScene& outputScene
	)
	{
		LGltfContextComponent* existContextComponent = dataContext.GetComponent(asset::LImportNodeDataType::ImportDataMesh);
		if (existContextComponent == nullptr)
		{
			LSharedPtr<LGltfContextComponentMesh> newContextComponent = MakeShared<LGltfContextComponentMesh>();
			dataContext.AddComponent(newContextComponent);
			existContextComponent = newContextComponent.get();
		}
		LGltfContextComponentMesh* meshContextComponent = static_cast<LGltfContextComponentMesh*>(existContextComponent);
		//计算模型的hash
		const LGltfDataMesh* gltfMeshData = static_cast<const LGltfDataMesh*>(gltfDataInput);
		LString meshHash = GenerateMeshHash(gltfMeshData);
		bool ifExist = meshContextComponent->Check(meshHash);
		if (ifExist)
		{
			for (int32_t nodeIndex = 0; nodeIndex < gltfNodesInput.size(); ++nodeIndex)
			{
				outputScene.ResetNodeData(gltfNodesInput[nodeIndex], asset::LImportNodeDataType::ImportDataMesh, meshContextComponent->Get(meshHash));
			}
			return;
		}
		//生成新的模型数据
		asset::LImportNodeDataMesh* nowOutData = outputScene.AddNewData<asset::LImportNodeDataMesh>();
		meshContextComponent->Insert(meshHash, nowOutData->GetId());
		size_t subMeshSize = gltfMeshData->GetSubMeshSize();
		for (size_t subMeshIndex = 0; subMeshIndex < subMeshSize; ++subMeshIndex)
		{
			nowOutData->AddSubMeshMessage(gltfDataInput->GetName(), subMeshIndex);
			const GltfSubmeshData& subMeshData = gltfMeshData->GetSubMesh(subMeshIndex);
			size_t vertexSize = subMeshData.mPosition.size() / 3;
			for (size_t vertexIndex = 0; vertexIndex < vertexSize; ++vertexIndex)
			{
				//position
				LVector3f positionData;
				memcpy(positionData.data(), &subMeshData.mPosition.data()[vertexIndex * 3], 3 * sizeof(float));
				//normal
				LVector3f normalData;
				normalData.setZero();
				if (subMeshData.mNormal.size() > 0)
				{
					memcpy(normalData.data(), &subMeshData.mNormal.data()[vertexIndex * 3], 3 * sizeof(float));
				}
				//tangent
				LVector4f tangentData;
				tangentData.setZero();
				if (subMeshData.mTangent.size() > 0)
				{
					memcpy(tangentData.data(), &subMeshData.mTangent.data()[vertexIndex * 4], 4 * sizeof(float));
				}
				//color
				LVector4f colorData;
				colorData.setZero();
				if (subMeshData.mColor.size() > 0)
				{
					uint32_t color = subMeshData.mColor[vertexIndex];
					byte* pointer = reinterpret_cast<byte*>(&color);
					colorData.x() = (float)pointer[0] / 255.0f;
					colorData.y() = (float)pointer[1] / 255.0f;
					colorData.z() = (float)pointer[2] / 255.0f;
					colorData.w() = (float)pointer[3] / 255.0f;
				}
				//uv
				LArray<LVector2f> uvData;
				if (subMeshData.mUv0.size() > 0)
				{
					LVector2f uv0Data;
					memcpy(uv0Data.data(), &subMeshData.mUv0.data()[vertexIndex * 2], 2 * sizeof(float));
					uvData.push_back(uv0Data);
				}
				if (subMeshData.mUv1.size() > 0)
				{
					LVector2f uv1Data;
					memcpy(uv1Data.data(), &subMeshData.mUv1.data()[vertexIndex * 2], 2 * sizeof(float));
					uvData.push_back(uv1Data);
				}
				nowOutData->AddFullVertex(subMeshIndex, positionData, normalData, tangentData, uvData, colorData);
			}
			size_t faceIndexSize = subMeshData.mFaceIndex.size();
			for (size_t faceIndex = 0; faceIndex < faceIndexSize; ++faceIndex)
			{
				nowOutData->AddFaceIndexDataToSubmesh(subMeshIndex, subMeshData.mFaceIndex[faceIndex]);
			}
		}
	}

}