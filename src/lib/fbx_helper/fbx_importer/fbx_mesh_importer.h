#pragma once
#include "fbx_importer_common.h"
#include "../fbx_loader/fbx_mesh_loader.h"
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
		LVector<fbxsdk::FbxVector2> mUvs;
	};

	struct VertexDataFullCombine
	{
		LVector<VertexDataFullMember> mVertexs;
	};

	class LFbxImporterMesh : public LFbxImporterBase
	{
	public:
		LFbxImporterMesh() {};
	private:
		void ParsingDataImpl(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene) override;

		void CombineVertexData(const LFbxDataMesh* meshData, LVector<VertexDataFullCombine>& vertexCombineData);

		void OptimizeCombineVert(
			const LVector<VertexDataFullCombine>& vertexCombineDataIn,
			LVector<VertexDataFullCombine>& vertexCombineDataOut,
			LVector<LVector<int32_t>>&OptimizeIndex
		);
	};
}