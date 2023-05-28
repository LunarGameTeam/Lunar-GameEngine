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
		const LArray<LString>& nodeList = skeletonData->GetSkeletonNodes();
		LArray<LString> SkeletonBoneNames;
		for (int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			auto name = nodeList[nodeId];
			SkeletonBoneNames.push_back(name);
		}
		sort(SkeletonBoneNames.begin(), SkeletonBoneNames.end(), [&](const LString& a, const LString& b)->bool {
			return a.Compare(b) > 0;
			}
		);
		for (int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			skeletonHash.Append(SkeletonBoneNames[nodeId]);
			skeletonHash.Append("#-#");
		}
	}

	void LFbxImporterSkeleton::ParsingDataImpl(
		const size_t nodeIdex,
		const LFbxDataBase* fbxDataInput,
		const LFbxSceneData* fbxScene,
		LFbxImportContext& dataContext,
		asset::LImportScene& outputScene
	)
	{
		const LFbxNodeBase fbxNodeInput = fbxScene->mNodes[nodeIdex];
		//先获取骨骼数据的上下文信息
		LFbxContextComponent* existContextComponent = dataContext.GetComponent(asset::LImportNodeDataType::ImportDataSkeleton);
		if (existContextComponent == nullptr)
		{
			LSharedPtr<LFbxContextComponentSkeleton> newContextComponent = MakeShared<LFbxContextComponentSkeleton>();
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
			return;
		}
		//不重复则需要添加一个新的skeleton
		asset::LImportNodeDataSkeleton* nowOutData = outputScene.AddNewData<asset::LImportNodeDataSkeleton>();
		skeletonContextComponent->Insert(skeletonHashData, nowOutData->GetId());
		//填充skeleton信息

		const LArray<LString>& nodeList = skeletonData->GetSkeletonNodes();
		LUnorderedMap<LString, int32_t> allNodes;
		for(int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			allNodes.insert({ nodeList[nodeId],nodeId });
		}
		for (int32_t nodeId = 0; nodeId < (int32_t)nodeList.size(); ++nodeId)
		{
			asset::SkeletonBoneData newBoneData;
			newBoneData.mBoneName = nodeList[nodeId];
			newBoneData.mLocalId = nodeId;
			auto nodeValueIdFind = fbxScene->mNodesNameRef.find(newBoneData.mBoneName);
			const LFbxNodeBase& nodeValueFind = fbxScene->mNodes[nodeValueIdFind->second];
			if (nodeValueFind.mParent == size_t(-1))
			{
				newBoneData.mParentId = -1;
			}
			const LFbxNodeBase& nodeParent = fbxScene->mNodes[nodeValueFind.mParent];
			if(allNodes.find(nodeParent.mName) == allNodes.end())
			{
				newBoneData.mParentId = -1;
			}
			else
			{
				newBoneData.mParentId = allNodes[nodeParent.mName];
			}
			FbxVector3ToVector3(nodeValueFind.mLocalTranslation, newBoneData.mInitLocation);
			FbxEulaToQuaternion(nodeValueFind.mLocalRotation, newBoneData.mInitRotation);
			FbxVector3ToVector3(nodeValueFind.mLocalScaling, newBoneData.mInitScal);
			nowOutData->AddBoneToData(newBoneData);
		}

	}
}