#define RESOURCE_IMPORT_EXPORT
#include"import_scene.h"
namespace luna::ImportData
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
}