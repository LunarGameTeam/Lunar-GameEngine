#pragma once
#include"import_data_common.h"
namespace luna::resimport
{
	struct LImportSubmesh
	{
		size_t mIndex;
		LString mName;
		LVector<uint32_t> mIndices;
		size_t mMaterialUse;

		LVector<LVector3f> mVertexPosition;
		LVector<LVector3f> mVertexNormal;
		LVector<LVector4f> mVertexTangent;
		LVector<LVector<LVector2f>> mVertexUv;
		LVector<LVector4f> mVertexColor;
	};

	class RESOURCE_IMPORT_API LImportNodeDataMesh : public LImportNodeDataBase
	{
		LVector<LImportSubmesh> mSubmesh;
	public:
		LImportNodeDataMesh(const size_t index);

		void AddFullVertex(
			const size_t subMeshIndex,
			const LVector3f pos,
			const LVector3f norm,
			const LVector4f tangent,
			const LVector<LVector2f> uv,
			const LVector4f color
		);

		size_t AddSubMeshMessage(const LString& name, const size_t materialUse);

		void AddFaceIndexDataToSubmesh(const size_t subMeshIndex, const uint32_t index);
	private:
		void ConvertDataAxisAndUnitImpl(LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) override;
	};

	//struct LImportNodeDataMaterial : public LImportNodeDataBase
	//{
	//	LUnorderedMap<LString, LString> mMaterialParam;
	//public:
	//	LImportNodeDataMaterial(const size_t index) :LImportNodeDataBase(LImportNodeDataType::ImportDataMaterial, index) {};
	//};
}

