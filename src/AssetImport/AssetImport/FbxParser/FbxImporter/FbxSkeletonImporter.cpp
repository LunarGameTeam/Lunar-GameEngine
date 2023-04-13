#pragma once
#include "AssetImport/FbxParser/FbxImporter/FbxSkeletonImporter.h"
using namespace fbxsdk;
namespace luna::lfbx
{
	void LFbxContextComponentSkeleton::Insert(const LString& skeletonHash, size_t skeletonDataId)
	{
		mExistSkeletonMap.insert({ skeletonHash ,skeletonDataId });
	}

	bool LFbxContextComponentSkeleton::Check(const LString& skeletonHash)
	{
		if(mExistSkeletonMap.find(skeletonHash) != mExistSkeletonMap.end())
		{
			return true;
		}
		return false;
	}

	size_t LFbxContextComponentSkeleton::Get(const LString& skeletonHash)
	{
		auto data = mExistSkeletonMap.find(skeletonHash);
		if (data != mExistSkeletonMap.end())
		{
			return data->second;
		}
		return (size_t)-1;
	}

	void LFbxImporterSkeleton::GetFbxSkeletonHash(const LFbxDataSkeleton* skeletonData, LString& skeletonHash)
	{
		const LArray<const FbxNode*>& nodeList = skeletonData->GetSkeletonNodes();
		LArray<LString> SkeletonBoneNames;
		for (int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			SkeletonBoneNames.push_back(nodeList[nodeId]->GetName());
		}
		sort(SkeletonBoneNames.begin(), SkeletonBoneNames.end(), [&](const LString& a, const LString& b)->bool {
			return a.Compare(b) > 0;
			}
		);
		for (int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			skeletonHash.Append(SkeletonBoneNames[nodeId]);
		}
	}

	void LFbxImporterSkeleton::ParsingDataImpl(
		const size_t nodeIdex,
		const LFbxDataBase* fbxDataInput,
		const LFbxNodeBase& fbxNodeInput,
		LFbxImportContext& dataContext,
		asset::LImportScene& outputScene
	)
	{
		//先获取骨骼数据的上下文信息
		LFbxContextComponent* existContextComponent = dataContext.GetComponent(asset::LImportNodeDataType::ImportDataSkeleton);
		if (existContextComponent == nullptr)
		{
			LSharedPtr<LFbxContextComponentSkeleton> newContextComponent = MakeShared<LFbxContextComponentSkeleton>(asset::LImportNodeDataType::ImportDataSkeleton);
			dataContext.AddComponent(newContextComponent);
			existContextComponent = newContextComponent.get();
		}
		LFbxContextComponentSkeleton* skeletonContextComponent = static_cast<LFbxContextComponentSkeleton*>(existContextComponent);
		//检查骨骼信息是否重复
		const LFbxDataSkeleton* skeletonData = static_cast<const LFbxDataSkeleton*>(fbxDataInput);
		LString skeletonHashData;
		GetFbxSkeletonHash(skeletonData, skeletonHashData);
		bool ifExist = skeletonContextComponent->Check(skeletonHashData);
		if (ifExist)
		{
			outputScene.ResetNodeData(nodeIdex,asset::LImportNodeDataType::ImportDataSkeleton,skeletonContextComponent->Get(skeletonHashData));
		}
		//不重复则需要添加一个新的skeleton
		asset::LImportNodeDataSkeleton* nowOutData = outputScene.AddNewData<asset::LImportNodeDataSkeleton>();
		skeletonContextComponent->Insert(skeletonHashData, nowOutData->GetId());
		//填充skeleton信息

		const LArray<const FbxNode*>& nodeList = skeletonData->GetSkeletonNodes();
		LUnorderedMap<LString, int32_t> allNodes;
		for(int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			allNodes.insert({ nodeList[nodeId]->GetName(),nodeId });
		}
		for (int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			asset::SkeletonBoneData newBoneData;
			newBoneData.mBoneName = nodeList[nodeId]->GetName();
			newBoneData.mLocalId = nodeId;
			const FbxNode* nodeParent =  nodeList[nodeId]->GetParent();
			if (nodeParent == nullptr)
			{
				newBoneData.mParentId = -1;
			}
			else if(allNodes.find(nodeParent->GetName()) == allNodes.end())
			{
				newBoneData.mParentId = -1;
			}
			else
			{
				newBoneData.mParentId = allNodes[nodeParent->GetName()];
			}
			FbxVector3ToVector3(nodeList[nodeId]->LclTranslation, newBoneData.mInitLocation);
			FbxEulaToQuaternion(nodeList[nodeId]->LclRotation, newBoneData.mInitRotation);
			FbxVector3ToVector3(nodeList[nodeId]->LclScaling, newBoneData.mInitScal);
		}

	}
}