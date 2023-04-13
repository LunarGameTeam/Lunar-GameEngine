#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxLoaderCommon.h"
#include "AssetImport/FbxParser/FbxLoader/FbxAnimationLoader.h"
namespace luna::lfbx
{
	struct LFbxSceneData
	{
		FbxAxisSystem mAxis;
		FbxSystemUnit mUnit;
		LArray<LFbxNodeBase> mNodes;
		LArray<std::shared_ptr<LFbxDataBase>> mDatas;
		LArray<LFbxAnimationStack> mAnimations;
	};

	class LFbxLoaderFactory
	{
		LUnorderedMap<LFbxDataType, std::shared_ptr<LFbxLoaderBase>> mResourceLoaders;
		LFbxAnimationLoader mAnimationLoders;
	public:

		LFbxLoaderFactory();

		std::shared_ptr<LFbxDataBase> LoadFbxData(LFbxDataType type, const LArray<LFbxNodeBase>& sceneNodes, FbxNode* pNode, LFbxLoadContext context);

		void LoadFbxAnimation(const FbxAnimStack* curAnimStack, LArray<LFbxAnimationStack>& outAnim, LFbxLoadContext context);
	};
	static std::shared_ptr<LFbxLoaderFactory> singleLoaderInterface = std::make_shared<LFbxLoaderFactory>();

	class LFbxLoaderHelper
	{
		struct NodeDataPack
		{
			size_t nodeIndex;
			FbxNode* pNode;
		};

		FbxManager* lSdkManager = NULL;

		FbxScene* lScene = NULL;
	public:
		void LoadFbxFile(const char* pFilename, LFbxSceneData& scene_out);
	private:
		void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);

		void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);

		bool LoadScene(FbxManager* pManager,FbxDocument* pScene, const char* pFilename);

		void InitSceneMessage(LFbxSceneData& sceneOut);

		void ProcessNode(
			size_t nodeParent,
			FbxNode* pNode,
			LFbxSceneData& sceneOut,
			std::unordered_map<LFbxDataType, LArray<NodeDataPack>> &nodeData
		);

		//FbxAMatrix ComputeNodeMatrix(FbxNode* Node);
		void ComputeLclTransform(
			FbxNode* pNode,
			FbxDouble3 &lclPosition,
			FbxDouble3 &lclRotation,
			FbxDouble3 &lclScale,
			FbxAMatrix& globalTransform
		);
	};
}