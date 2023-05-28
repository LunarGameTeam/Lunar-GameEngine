#pragma once
#include "AssetImport/FbxParser/FbxImporter/FbxImporterCommon.h"
#include "AssetImport/FbxParser/FbxLoader/FbxMeshLoader.h"
namespace luna::lfbx
{
	static const double deltaEpsion = 1.0e-7;

	struct VertexDataFullMember
	{
		size_t globelIndex;
		size_t baseIndex;
		fbxsdk::FbxVector4 mPosition;
		fbxsdk::FbxVector4 mNormal;
		fbxsdk::FbxVector4 mTangent;
		fbxsdk::FbxColor mColor;
		LArray<fbxsdk::FbxVector2> mUvs;
		LArray<uint32_t> mRefBones;
		LArray<float> mWeights;
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
		void ParsingDataImpl(
			const size_t nodeIdex,
			const LFbxDataBase* fbxDataInput,
			const LFbxSceneData* fbxScene,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		) override;

		void CombineVertexData(const LFbxDataMesh* meshData, LArray<VertexDataFullCombine>& vertexCombineData);

		void OptimizeCombineVert(
			const LArray<VertexDataFullCombine>& vertexCombineDataIn,
			LArray<VertexDataFullCombine>& vertexCombineDataOut,
			LArray<LArray<int32_t>>&OptimizeIndex
		);
	};
}