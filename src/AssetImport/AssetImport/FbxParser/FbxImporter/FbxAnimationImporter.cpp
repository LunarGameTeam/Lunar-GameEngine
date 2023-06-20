#pragma once
#include "FbxAnimationImporter.h"
using namespace fbxsdk;
namespace luna::lfbx
{
	void LFbxImporterSkeletonAnimation::TryGenerateSkeletonAnimation(
		const LFbxAnimationStack* fbxDataInput,
		const LUnorderedMap<LString, LArray<size_t>>& nodeTransformCurveMark,
		const asset::LImportNodeDataSkeleton* skeletonData,
		asset::LImportScene& outputScene
	)
	{
		const LArray<asset::SkeletonBoneData>& allBoneData = skeletonData->GetBones();
		bool ifHaveAnim = false;
		for(int32_t boneId = 0; boneId < allBoneData.size(); ++boneId)
		{
			auto curveBone = nodeTransformCurveMark.find(allBoneData[boneId].mBoneName);
			if (curveBone != nodeTransformCurveMark.end())
			{
				ifHaveAnim = true;
				break;
			}
		}
		if (!ifHaveAnim)
		{
			return;
		}

		asset::LImportNodeSkeletonAnimation* newAnimation = outputScene.AddNewAnimation<asset::LImportNodeSkeletonAnimation>();
		newAnimation->SetSkeletonIndex(skeletonData->GetId());
		for (int32_t boneId = 0; boneId < allBoneData.size(); ++boneId)
		{
			auto curveBone = nodeTransformCurveMark.find(allBoneData[boneId].mBoneName);
			if (curveBone == nodeTransformCurveMark.end())
			{
				continue;
			}
			for (int32_t curveId = 0; curveId < curveBone->second.size(); ++curveId)
			{
				std::shared_ptr<LFbxCurveBase> curveValue = fbxDataInput->GetCurves()[curveBone->second[curveId]];
				LFbxTemplateCurve<FbxDouble3>* newCurveTranslation = static_cast<LFbxTemplateCurve<FbxDouble3>*>(curveValue.get());
				FbxEulaCurveToImportData(newCurveTranslation, newAnimation);
			}
		}
		
	}

	void LFbxImporterSkeletonAnimation::FbxEulaCurveToImportData(const LFbxTemplateCurve<FbxDouble3>* fbxCurve, asset::LImportNodeSkeletonAnimation* newAnimation)
	{
		const LArray<FbxTime>& curveTimes = fbxCurve->GetTimes();
		const LArray<FbxDouble3>& curveValues = fbxCurve->GetValues();
		LFbxAnimationCurveType curveType = fbxCurve->GetType();
		for (size_t i = 0; i < curveTimes.size(); ++i)
		{
			float newTime = static_cast<float>(curveTimes[i].GetMilliSeconds());
			LVector3f newValue;
			FbxVector3ToVector3(curveValues[i], newValue);
			switch (curveType)
			{
				case luna::lfbx::LFbxAnimationCurveType::NODE_TRANSLATION:
				{
					newAnimation->AddCurveTranslationToNode(fbxCurve->GetName(), newTime, newValue);
					break;
				}
				case luna::lfbx::LFbxAnimationCurveType::NODE_ROTATION:
				{
					LQuaternion newRotation;
					FbxEulaToQuaternion(curveValues[i], newRotation);
					newAnimation->AddCurveRotationToNode(fbxCurve->GetName(), newTime, newRotation);
					break;
				}
				case luna::lfbx::LFbxAnimationCurveType::NODE_SCALING:
				{
					newAnimation->AddCurveScaleToNode(fbxCurve->GetName(), newTime, newValue);
					break;
				}
				default:
				{
					assert(false);
					break;
				}
			}
		}
	}

	void LFbxImporterSkeletonAnimation::ParsingAnimationImpl(
		const LFbxAnimationStack* fbxDataInput,
		LFbxImportContext& dataContext,
		asset::LImportScene& outputScene
	)
	{
		const LArray<std::shared_ptr<LFbxCurveBase>>& curves = fbxDataInput->GetCurves();
		LUnorderedMap<LString,LArray<size_t>> nodeTransformCurveMark;
		for (int32_t curveID = 0; curveID < curves.size(); ++curveID)
		{
			std::shared_ptr<LFbxCurveBase> eachCurve = curves[curveID];
			if (eachCurve->GetType() == LFbxAnimationCurveType::NODE_TRANSLATION || eachCurve->GetType() == LFbxAnimationCurveType::NODE_ROTATION || eachCurve->GetType() == LFbxAnimationCurveType::NODE_SCALING)
			{
				nodeTransformCurveMark[eachCurve->GetName()].push_back(curveID);
			}
		}
		LArray<size_t> allSkeletonIds = outputScene.FilterDataByType(asset::LImportNodeDataType::ImportDataSkeleton);
		for (int32_t skeletonId : allSkeletonIds)
		{
			const asset::LImportNodeDataSkeleton* skeletonData = outputScene.GetData<asset::LImportNodeDataSkeleton>(skeletonId);
			TryGenerateSkeletonAnimation(fbxDataInput, nodeTransformCurveMark, skeletonData, outputScene);
		}
	}
}