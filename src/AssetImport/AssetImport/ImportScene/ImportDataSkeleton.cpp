#pragma once
#include "AssetImport/ImportScene/ImportDataSkeleton.h"
namespace luna::asset
{
	LImportNodeDataSkeleton::LImportNodeDataSkeleton(const size_t index) : LImportNodeDataBase(LImportNodeDataType::ImportDataSkeleton,index)
	{
	}

	void LImportNodeDataSkeleton::AddBoneToData(const SkeletonBoneData& boneData)
	{
		skeletonBones.push_back(boneData);
	}

	void LImportNodeDataSkeleton::SortBoneData()
	{
		//�������Ӹ��ڵ�ļ�¼��ʽ��ת��Ϊ�ӽڵ��¼
		LArray<LArray<size_t>> childList;
		childList.resize(skeletonBones.size());
		for (auto eachValue : skeletonBones)
		{
			childList[eachValue.mParentId].push_back(eachValue.mLocalId);
		}
		//���ҹǼܵĸ��ڵ�
		size_t rootBoneId = 0;
		while (skeletonBones[rootBoneId].mParentId != -1)
		{
			rootBoneId = skeletonBones[rootBoneId].mParentId;
		}
		//ͨ�����������������˳��������򣬱�֤������ʼ�������ӹ���ǰ��
		LQueue<size_t> allDealBones;
		allDealBones.push(rootBoneId);
		LArray<SkeletonBoneData> skeletonBonesNew;
		while(!allDealBones.empty())
		{
			size_t nowIndex = allDealBones.front();
			allDealBones.pop();
			skeletonBonesNew.push_back(skeletonBones[nowIndex]);
			struct BoneIndexAndName
			{
				LString mBoneName;
				size_t mBoneIndex;
			};
			LArray<BoneIndexAndName> sortBoneArray;
			for (int32_t childIndex = 0; childIndex < childList[nowIndex].size(); ++childIndex)
			{
				BoneIndexAndName newPair;
				newPair.mBoneIndex = childList[nowIndex][childIndex];
				newPair.mBoneName = skeletonBones[newPair.mBoneIndex].mBoneName;
				sortBoneArray.push_back(newPair);
			}		
			std::sort(sortBoneArray.begin(), sortBoneArray.end(), [&](const BoneIndexAndName& a, const BoneIndexAndName& b)->bool {
				return a.mBoneName.Compare(b.mBoneName) > 0;
				}
			);
			for (int32_t childIndex = 0; childIndex < sortBoneArray.size(); ++childIndex)
			{
				allDealBones.push(sortBoneArray[childIndex].mBoneIndex);
			}
		}
		skeletonBones.clear();
		skeletonBones = skeletonBonesNew;
	}

	void LImportNodeDataSkeleton::ConvertDataAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		for (auto &eachValue : skeletonBones)
		{
			//�任loacation����
			ConvertPositionByTransform(hasReflectTransform, convertInvMatrix, convertMatrix,eachValue.mInitLocation);
			//�任scal����
			ConvertScaleByTransform(hasReflectTransform, convertInvMatrix, convertMatrix, eachValue.mInitScal);
			//�任rotation����
			ConvertRotationByTransform(hasReflectTransform, convertInvMatrix, convertMatrix, eachValue.mInitRotation);
		}
	}

}

