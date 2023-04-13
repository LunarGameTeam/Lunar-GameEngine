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
		// ����ǰSkin�е�ÿ��Cluster����Ӧ��Skeleton��
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			const FbxCluster* pCluster = pSkin->GetCluster(clusterIndex);
			//�޳�������������skin cluster
			LString clusterUserId = pCluster->GetUserDataID();
			if (!pCluster || clusterUserId == "Maya_ClusterHint" || clusterUserId == "CompensationCluster")
			{
				continue;
			}
			//��ȡcluster���ӵĹ����ڵ�
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
		//�ȼ�¼��Ƥ����Ĺ���
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
		//�Ƶ������еĸ�����
		LUnorderedMap<LString, const FbxNode*> allRootNode;
		GenerateRootBoneFromSkinBone(directLinkNode, allRootNode, context);
		//����Ƿ��ж����
		if (allRootNode.size() > 1)
		{
			newSkeleton->MarkMultiRoot();
		}
		//������չ�����еĹ������ϣ����ɹǼ�
		GenerateSkeletonTree(allRootNode, newSkeleton.get(), context);
		return newSkeleton;
	};
}