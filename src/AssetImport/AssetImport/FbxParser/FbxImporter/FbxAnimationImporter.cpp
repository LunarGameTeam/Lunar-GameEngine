#pragma once
#include "FbxAnimationImporter.h"

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

		asset::LImportNodeDataSkeletonAnimation* newAnimation = outputScene.AddNewAnimation<asset::LImportNodeDataSkeletonAnimation>();
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
				switch (curveValue->GetType())
				{
					case LFbxAnimationCurveType::NODE_TRANSLATION:
					{
						LFbxTemplateCurve<FbxVector4>* newCurveTranslation = static_cast<LFbxTemplateCurve<FbxVector4>*>(curveValue.get());
						break;
					}
					case LFbxAnimationCurveType::NODE_ROTATION:
					{
						LFbxTemplateCurve<FbxQuaternion>* newCurveRotation = static_cast<LFbxTemplateCurve<FbxQuaternion>*>(curveValue.get());
						break;
					}
					case LFbxAnimationCurveType::NODE_SCALING:
					{
						LFbxTemplateCurve<FbxVector4>* newCurveScaling = static_cast<LFbxTemplateCurve<FbxVector4>*>(curveValue.get());
						break;
					}
					default:
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