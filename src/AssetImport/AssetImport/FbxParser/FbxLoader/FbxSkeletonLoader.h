#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxLoaderCommon.h"
namespace luna::lfbx
{
	class LFbxDataSkeleton : public LFbxDataBase
	{
		bool HasMultiRoot = false;
		LArray<LString> skeletonBoneGroup;
	public:
		LFbxDataSkeleton() : LFbxDataBase(LFbxDataType::FbxSkeletonData) {};
		inline void MarkMultiRoot() { HasMultiRoot = true; }
		inline void AddNodeToGroup(const FbxNode* nodeData) { skeletonBoneGroup.push_back(nodeData->GetName()); };
		inline const LArray<LString>& GetSkeletonNodes() const{ return skeletonBoneGroup; };
	};

	class LFbxLoaderSkeleton : public LFbxLoaderBase
	{
	public:
		LFbxLoaderSkeleton() {};
	private:
		std::shared_ptr<LFbxDataBase> ParsingDataImpl(const LArray<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, LFbxLoadContext context) override;
		
		void GenerateSkeletonFromSkin(const FbxSkin* pSkin, LUnorderedMap<LString, const FbxNode*>& directLinkNode);

		void GenerateRootBoneFromSkinBone(
			const LUnorderedMap<LString, const FbxNode*>& directLinkNode,
			LUnorderedMap<LString, const FbxNode*>& rootNode,
			LFbxLoadContext context
		);

		void GenerateSkeletonTree(
			const LUnorderedMap<LString, const FbxNode*>& rootNode,
			LFbxDataSkeleton* skeletonData,
			LFbxLoadContext context
		);
	};
}