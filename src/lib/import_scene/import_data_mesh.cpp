#pragma once
#define RESOURCE_IMPORT_EXPORT
#include"import_data_mesh.h"
namespace luna::resimport
{
	LImportNodeDataMesh::LImportNodeDataMesh(const size_t index) :LImportNodeDataBase(LImportNodeDataType::ImportDataMesh, index)
	{

	};

	void LImportNodeDataMesh::AddFullVertex(
		const size_t subMeshIndex,
		const LVector3f pos,
		const LVector3f norm,
		const LVector4f tangent,
		const LVector<LVector2f> uv,
		const LVector4f color
	)
	{
		mSubmesh[subMeshIndex].mVertexPosition.push_back(pos);
		mSubmesh[subMeshIndex].mVertexNormal.push_back(norm);
		mSubmesh[subMeshIndex].mVertexTangent.push_back(tangent);
		mSubmesh[subMeshIndex].mVertexUv.push_back(uv);
		mSubmesh[subMeshIndex].mVertexColor.push_back(color);
	};

	size_t LImportNodeDataMesh::AddSubMeshMessage(
		const LString& name,
		const size_t materialUse
	)
	{
		LImportSubmesh newSubmesh;
		newSubmesh.mName = name;
		newSubmesh.mIndex = mSubmesh.size();
		newSubmesh.mMaterialUse = materialUse;
		mSubmesh.push_back(newSubmesh);
		return newSubmesh.mIndex;
	}

	void LImportNodeDataMesh::AddFaceIndexDataToSubmesh(const size_t subMeshIndex, const uint32_t index)
	{
		if (subMeshIndex >= mSubmesh.size())
		{
			return;
		};
		mSubmesh[subMeshIndex].mIndices.push_back(index);
	}

	void LImportNodeDataMesh::ConvertDataAxisAndUnitImpl(LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		for (size_t submeshID = 0; submeshID < mSubmesh.size(); ++submeshID)
		{
			LImportSubmesh& subMeshData = mSubmesh[submeshID];
			for (size_t vertexID = 0; vertexID < subMeshData.mVertexPosition.size(); ++vertexID)
			{
				//变换position数据
				LTransform translationMid;
				translationMid.setIdentity();
				translationMid.translate(subMeshData.mVertexPosition[vertexID]);
				LMatrix4f transDataAns = convertInvMatrix * translationMid.matrix() * convertMatrix;
				subMeshData.mVertexPosition[vertexID] = transDataAns.block<3, 1>(0, 3);
				//变换normal数据
				translationMid.setIdentity();
				translationMid.translate(subMeshData.mVertexNormal[vertexID]);
				transDataAns = convertInvMatrix * translationMid.matrix() * convertMatrix;
				subMeshData.mVertexNormal[vertexID] = transDataAns.block<3, 1>(0, 3);
				subMeshData.mVertexNormal[vertexID].normalize();
				//变换tangent数据
				LVector3f tangentValue = subMeshData.mVertexTangent[vertexID].head<3>();
				translationMid.setIdentity();
				translationMid.translate(tangentValue);
				transDataAns = convertInvMatrix * translationMid.matrix() * convertMatrix;
				tangentValue = transDataAns.block<3, 1>(0, 3);
				tangentValue.normalize();
				float bnormalSign = subMeshData.mVertexTangent[vertexID].w();
				subMeshData.mVertexTangent[vertexID] = LVector4f(tangentValue.x(), tangentValue.y(), tangentValue.z(), bnormalSign);
			}
		}
	}
}

