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

		void GenerateMikkTspaceTangent();

		void ComputeVertexCache();

		inline size_t GetSubMeshSize()const
		{
			return mSubmesh.size();
		}

		inline const LImportSubmesh& GettSubMesh(size_t index)const 
		{
			return mSubmesh[index];
		}
	private:
		void ConvertDataAxisAndUnitImpl(LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) override;

		void CopyAndClearSubmeshVertex(LImportSubmesh &src, LImportSubmesh& dst);

		void CopyVertexData(LImportSubmesh& src, LImportSubmesh& dst,size_t srcId,size_t dstId);
	};

	//struct LImportNodeDataMaterial : public LImportNodeDataBase
	//{
	//	LUnorderedMap<LString, LString> mMaterialParam;
	//public:
	//	LImportNodeDataMaterial(const size_t index) :LImportNodeDataBase(LImportNodeDataType::ImportDataMaterial, index) {};
	//};
}

