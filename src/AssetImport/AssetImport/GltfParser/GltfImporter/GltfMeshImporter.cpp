#pragma once
#include "AssetImport/GltfParser/GltfImporter/GltfMeshImporter.h"
#include "AssetImport/GltfParser/GltfLoader/GltfMeshLoader.h"

namespace luna::asset
{

void LGltfImporterMesh::ParsingDataImpl(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, LImportScene& outputScene)
{
	asset::LImportNodeDataMesh* nowOutData = outputScene.AddNewData<asset::LImportNodeDataMesh>();
	const LGltfDataMesh* gltfMeshData = static_cast<const LGltfDataMesh*>(gltfDataInput);
	size_t subMeshSize = gltfMeshData->GetSubMeshSize();
	for (size_t subMeshIndex = 0; subMeshIndex < subMeshSize; ++subMeshIndex)
	{
		nowOutData->AddSubMeshMessage(gltfNodeInput.mName, subMeshIndex);
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