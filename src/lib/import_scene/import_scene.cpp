#include"import_scene.h"
namespace luna::ImportData
{
	void LImportNodeDataMesh::AddFullVertex(const LVector3f pos, const LVector3f norm, const LVector4f tangent, const LVector<LVector2f> uv, const LVector4f color)
	{
		mVertexPosition.push_back(pos);
		mVertexNormal.push_back(norm);
		mVertexTangent.push_back(tangent);
		mVertexUv.push_back(uv);
		mVertexColor.push_back(color);
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