#pragma once
#include<fbxsdk.h>
#include "mikktspace.h"
#include "forsythtriangleorderoptimizer.h"
#include "core/core_library.h"
#include "render/asset/mesh_asset.h"
namespace luna
{
	struct FBXMeshFace
	{
		LVector<render::BaseVertex> FBXMeshVertex;
		LVector<uint32_t> FBXMeshVertexIndex;
		uint32_t mat_id = 0;
	};
	struct FBXMeshNode
	{
		int32_t uv_channel_size;
		bool if_divide_by_mat = false;
		LVector<FBXMeshFace> mesh_face_data;
		LString submesh_name;
		LVector<LString> material_names;
	};
	struct LFbxSceneData
	{
		LVector<FBXMeshNode> mesh_data;

	};
	class LFbxImporterHelper
	{
		FbxManager* lSdkManager = NULL;
		FbxScene* lScene = NULL;
	public:
		void LoadFbxFile(const char* pFilename, LFbxSceneData& scene_out);
	private:
		void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
		void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
		bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
		void InitSceneMessage(bool change_unit = false);
		void ProcessNode(FbxNode* pNode, LFbxSceneData& scene_out);
		void ProcessMesh(FbxNode* pNode, FBXMeshNode& new_mesh);
		FbxAMatrix ComputeNodeMatrix(FbxNode* Node);
		LString GetFbxObjectName(const FbxObject* Object);
		void ReadVertex(FbxAMatrix& globel_matrix, FbxMesh* pMesh, int ctrlPointIndex, LVector3f* pVertex);
		void ReadColor(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, LVector4f* pColor);
		void ReadUV(FbxMesh* pMesh, int ctrlPointIndex, int textureUVIndex, int uvLayer, LVector2f* pUV);
		void ReadNormal(FbxAMatrix& globel_matrix, FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, LVector3f* pNormal);
		void ReadTangent(FbxAMatrix& globel_matrix, FbxMesh* pMesh, int ctrlPointIndex, int vertecCounter, LVector4f* pTangent);
		void ReadMaterialIndex(FbxMesh* pMesh, int faceIndex, uint32_t* mat_id);
	};

	struct SimpleMesh
	{
		LString submesh_name;
		LString material_using;
		LVector<render::BaseVertex> vertex;
		LVector<uint32_t> indices;
	};
	class LFbxMeshAssetHelper
	{
	public:
		LFbxMeshAssetHelper() {};

		void ExchangeFbxSceneToLunaMesh(const LFbxSceneData& scene_in, render::MeshAsset* m_mesh);
	private:
		void DivideFbxMesh(const FBXMeshNode& mesh_node_in, LVector<FBXMeshNode>& mesh_node_out);

		void ProfileFbxMesh(const FBXMeshNode& mesh_node_in, SimpleMesh& simple_mesh);

		void PostProcessingMesh(SimpleMesh& mesh_in);

		void CombineLunaMesh(LVector<SimpleMesh>& simple_mesh, render::MeshAsset* mesh_out);

		bool IfEqual(const LVector2f& in_a, const LVector2f& in_b);

		bool IfEqual(const LVector3f& in_a, const LVector3f& in_b);

		bool IfEqual(const LVector4f& in_a, const LVector4f& in_b);

		bool IfEqual(
			const render::BaseVertex& in_a,
			const render::BaseVertex& in_b,
			const int32_t& uv_channel_size,
			const bool& if_color = false
		);

		void ComputeMikkTspace(SimpleMesh& simple_mesh);

		void ComputeVertexCache(SimpleMesh& simple_mesh);

		const float kinda_small_number = 1.e-4f;
	};

}

