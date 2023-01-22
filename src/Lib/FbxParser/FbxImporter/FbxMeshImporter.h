#pragma once
#include "FbxImporter/FbxImporterCommon.h"
#include "FbxLoader/FbxMeshLoader.h"
namespace luna::lfbx
{
	static const double deltaEpsion = 1.0e-7;

	struct VertexDataFullMember
	{
		size_t baseIndex;
		fbxsdk::FbxVector4 mPosition;
		fbxsdk::FbxVector4 mNormal;
		fbxsdk::FbxVector4 mTangent;
		fbxsdk::FbxColor mColor;
		LArray<fbxsdk::FbxVector2> mUvs;
	};

	struct VertexDataFullCombine
	{
		LArray<VertexDataFullMember> mVertexs;
	};

	class LFbxImporterMesh : public LFbxImporterBase
	{
	public:
		LFbxImporterMesh() {};
	private:
		void ParsingDataImpl(const LFbxDataBase* fbxDataInput, const LFbxNodeBase& fbxNodeInput, resimport::LImportScene& outputScene) override;

		void CombineVertexData(const LFbxDataMesh* meshData, LArray<VertexDataFullCombine>& vertexCombineData);

		void OptimizeCombineVert(
			const LArray<VertexDataFullCombine>& vertexCombineDataIn,
			LArray<VertexDataFullCombine>& vertexCombineDataOut,
			LArray<LArray<int32_t>>&OptimizeIndex
		);
	};
}