#pragma once
#include"FbxLoader/FbxLoaderCommon.h"
namespace luna::lfbx
{
	struct LFbxSceneData
	{
		fbxsdk::FbxAxisSystem mAxis;
		fbxsdk::FbxSystemUnit mUnit;
		LArray<LFbxNodeBase> mNodes;
		LArray<std::shared_ptr<LFbxDataBase>> mDatas;
	};

	class LFbxLoaderFactory
	{
		LUnorderedMap<LFbxDataType, std::shared_ptr<LFbxLoaderBase>> mLoaders;
	public:
		LFbxLoaderFactory();
		std::shared_ptr<LFbxDataBase> LoadFbxData(LFbxDataType type, const LArray<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager);
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
			std::unordered_map<LFbxDataType, LArray<NodeDataPack>> &nodeData
		);

		//fbxsdk::FbxAMatrix ComputeNodeMatrix(fbxsdk::FbxNode* Node);
		void ComputeLclTransform(
			fbxsdk::FbxNode* pNode,
			FbxDouble3 &lclPosition,
			FbxDouble3 &lclRotation,
			FbxDouble3 &lclScale,
			fbxsdk::FbxAMatrix& globalTransform
		);
	};
}