#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxSkeletonLoader.h"
using namespace fbxsdk;
namespace luna::lfbx
{
	void LFbxLoaderSkeleton::GenerateSkeletonFromSkin(const FbxSkin* pSkin, LUnorderedMap<LString, const FbxNode*> &directLinkNode)
	{
		if (!pSkin)
		{
			return;
		}
		FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;
		int clusterCount = pSkin->GetClusterCount();
		// 处理当前Skin中的每个Cluster（对应到Skeleton）
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			const FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);
			//剔除不符合条件的skin cluster
			LString clusterUserId = pCluster->GetUserDataID();
			if (!pCluster || clusterUserId == "Maya_ClusterHint" || clusterUserId == "CompensationCluster")
			{
				continue;
			}
			//获取cluster链接的骨骼节点
			const FbxNode* pLinkNode = pCluster->GetLink();
			if (!pLinkNode)
			{
				continue;
			}
			LString nodeName = pLinkNode->GetName();
			auto existNode = directLinkNode.find(nodeName);
			if (existNode != directLinkNode.end())
			{
				assert(false);
			}
			directLinkNode.insert({ nodeName ,pLinkNode });
		}
	}

	void LFbxLoaderSkeleton::GenerateRootBoneFromSkinBone(
		const LUnorderedMap<LString, const FbxNode*>& directLinkNode,
		LUnorderedMap<LString, const FbxNode*>& rootNode,
		LFbxLoadContext context
	)
	{
		for (auto eachNode : directLinkNode)
		{
			const FbxNode* dealNode = eachNode.second;
			while (dealNode->GetParent() != context.lScene->GetRootNode())
			{
				dealNode = dealNode->GetParent();
			}
			LString rootName = dealNode->GetName();
			if(rootNode.find(rootName) == rootNode.end())
			{
				rootNode.insert({ rootName,dealNode });
			}
		}
	}

	void LFbxLoaderSkeleton::GenerateSkeletonTree(
		const LUnorderedMap<LString, const FbxNode*>& rootNode,
		LFbxDataSkeleton* skeletonData,
		LFbxLoadContext context
	)
	{
		for (auto eachNode : rootNode)
		{
			LQueue<const FbxNode*> bfsList;
			bfsList.push(eachNode.second);
			while (!bfsList.empty())
			{
				const FbxNode* nodeFront = bfsList.front();
				bfsList.pop();
				skeletonData->AddNodeToGroup(nodeFront);
				int childCount = nodeFront->GetChildCount();
				for(int childIndex = 0; childIndex < childCount; ++childIndex)
				{
					bfsList.push(nodeFront->GetChild(childIndex));
				}
			}
		}
	}

	std::shared_ptr<LFbxDataBase> LFbxLoaderSkeleton::ParsingDataImpl(const LArray<LFbxNodeBase>& sceneNodes, FbxNode* pNode, LFbxLoadContext context)
	{
		std::shared_ptr<LFbxDataSkeleton> newSkeleton = std::make_shared<LFbxDataSkeleton>();
		FbxMesh* pMesh = pNode->GetMesh();
		if (pMesh == NULL)
		{
			return nullptr;
		}
		//先记录蒙皮参与的骨骼
		int deformerCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
		LUnorderedMap<LString, const FbxNode*> directLinkNode;
		for (int i = 0; i < deformerCount; ++i)
		{
			const FbxSkin* pFBXSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
			if (pFBXSkin == nullptr)
			{
				continue;
			}
			GenerateSkeletonFromSkin(pFBXSkin, directLinkNode);
		}
		//推导出所有的根骨骼
		LUnorderedMap<LString, const FbxNode*> allRootNode;
		GenerateRootBoneFromSkinBone(directLinkNode, allRootNode, context);
		//标记是否有多个根
		if (allRootNode.size() > 1)
		{
			newSkeleton->MarkMultiRoot();
		}
		//接下来展开所有的骨骼集合，生成骨架
		GenerateSkeletonTree(allRootNode, newSkeleton.get(), context);
		return newSkeleton;
	};
}