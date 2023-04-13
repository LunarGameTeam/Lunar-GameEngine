#pragma once

#include "AssetImport/ImportScene/ImportDataCommon.h"

namespace luna::asset
{
	struct ASSET_IMPORT_API LImportSubmesh
	{
		size_t mIndex;
		LString mName;
		LArray<uint32_t> mIndices;
		size_t mMaterialUse;

		LArray<LVector3f> mVertexPosition;
		LArray<LVector3f> mVertexNormal;
		LArray<LVector4f> mVertexTangent;
		LArray<LArray<LVector2f>> mVertexUv;
		LArray<LVector4f> mVertexColor;
		LArray<uint32_t>  mRefBone;
		LArray<float>     mWeight;

		LArray<LString>   mRefBoneName;
		LArray<LMatrix4f> mRefBonePose;
	
	};

	class ASSET_IMPORT_API LImportNodeDataMesh : public LImportNodeDataBase
	{
		LArray<LImportSubmesh> mSubmesh;
	public:
		LImportNodeDataMesh(const size_t index);

		void AddFullVertex(
			const size_t subMeshIndex,
			const LVector3f pos,
			const LVector3f norm,
			const LVector4f tangent,
			const LArray<LVector2f> uv,
			const LVector4f color
		);

		void AddFullSkin(
			const size_t subMeshIndex,
			const LArray<uint32_t> &mRefBone,
			const LArray<float> &mWeight
		);

		size_t AddSubMeshMessage(const LString& name, const size_t materialUse);

		void AddFaceIndexDataToSubmesh(const size_t subMeshIndex, const uint32_t index);

		void AddBoneMessageToSubmesh(const size_t subMeshIndex, const LString &boneName, const LMatrix4f& bonePose);

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
		void ConvertDataAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) override;

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

