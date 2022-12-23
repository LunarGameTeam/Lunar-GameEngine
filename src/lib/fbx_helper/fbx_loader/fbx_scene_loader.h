#pragma once
#include"fbx_loader_common.h"
namespace luna::lfbx
{
	struct LFbxSceneData
	{
		FbxAxisSystem mAxis;
		FbxSystemUnit mUnit;
		LVector<LFbxNodeBase> mNodes;
		LVector<std::shared_ptr<LFbxDataBase>> mDatas;
	};

	class LFbxLoaderFactory
	{
		LUnorderedMap<LFbxDataType, std::shared_ptr<LFbxLoaderBase>> mLoaders;
	public:
		LFbxLoaderFactory();
		std::shared_ptr<LFbxDataBase> LoadFbxData(LFbxDataType type, const LVector<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager);
	};
	static std::shared_ptr<LFbxLoaderFactory> singleLoaderInterface = std::make_shared<LFbxLoaderFactory>();

	class LFbxLoaderHelper
	{
		struct NodeDataPack
		{
			size_t nodeIndex;
			fbxsdk::FbxNode* pNode;
		};

		fbxsdk::FbxManager* lSdkManager = NULL;

		fbxsdk::FbxScene* lScene = NULL;
	public:
		void LoadFbxFile(const char* pFilename, LFbxSceneData& scene_out);
	private:
		void InitializeSdkObjects(fbxsdk::FbxManager*& pManager, fbxsdk::FbxScene*& pScene);

		void DestroySdkObjects(fbxsdk::FbxManager* pManager, bool pExitStatus);

		bool LoadScene(fbxsdk::FbxManager* pManager,fbxsdk::FbxDocument* pScene, const char* pFilename);

		void InitSceneMessage(LFbxSceneData& sceneOut);

		void ProcessNode(
			size_t nodeParent,
			fbxsdk::FbxNode* pNode,
			LFbxSceneData& sceneOut,
			std::unordered_map<LFbxDataType, LVector<NodeDataPack>> &nodeData
		);

		fbxsdk::FbxAMatrix ComputeNodeMatrix(fbxsdk::FbxNode* Node);
	};
}