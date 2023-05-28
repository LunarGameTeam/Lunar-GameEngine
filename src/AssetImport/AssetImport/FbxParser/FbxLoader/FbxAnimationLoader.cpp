#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxAnimationLoader.h"
using namespace fbxsdk;
namespace luna::lfbx
{
	FbxTimeSpan LFbxAnimationLoader::GetAnimationTimeSpan(const LUnorderedMap<LString, fbxsdk::FbxNode*> &allNodeOut, const FbxAnimStack* AnimStack)
	{
		//这里暂时不考虑动画轨道中存在废尾部数据需要裁剪的情况
		FbxTimeSpan AnimTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
		AnimTimeSpan = AnimStack->GetLocalTimeSpan();

		return AnimTimeSpan;
	}
	
	bool LFbxAnimationLoader::CheckNodeHasTransformAnimation(fbxsdk::FbxNode* targetNode, FbxAnimLayer* animLayer)
	{
		LArray<FbxAnimCurve*> newCurves;
		newCurves.resize(9);
		newCurves[0] = targetNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
		newCurves[1] = targetNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
		newCurves[2] = targetNode->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
		newCurves[3] = targetNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
		newCurves[4] = targetNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
		newCurves[5] = targetNode->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
		newCurves[6] = targetNode->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
		newCurves[7] = targetNode->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
		newCurves[8] = targetNode->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
		for (int32_t i = 0; i < 9; ++i)
		{
			if (newCurves[i] != nullptr)
			{
				return true;
			}
		}
		return false;
	}
	
	void LFbxAnimationLoader::GetNodeFromAnimLayer(fbxsdk::FbxNode* parentNode, FbxAnimLayer* animLayer, LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut)
	{
		if (CheckNodeHasTransformAnimation(parentNode, animLayer) && allNodeOut.find(parentNode->GetName()) == allNodeOut.end())
		{
			allNodeOut.insert({ parentNode->GetName() ,parentNode });
		}
		for (int i = 0; i < parentNode->GetChildCount(); ++i)
		{
			GetNodeFromAnimLayer(parentNode->GetChild(i), animLayer, allNodeOut);
		}
	}

	void LFbxAnimationLoader::FindAllAnimateNode(fbxsdk::FbxNode* rootNode, const FbxAnimStack* curAnimStack, LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut)
	{
		int32_t AnimStackLayerCount = curAnimStack->GetMemberCount();
		for (int32_t LayerIndex = 0; LayerIndex < AnimStackLayerCount; ++LayerIndex)
		{
			FbxAnimLayer* eachAnimLayer = (FbxAnimLayer*)curAnimStack->GetMember(LayerIndex);
			GetNodeFromAnimLayer(rootNode, eachAnimLayer, allNodeOut);
		}
	}

	void LFbxAnimationLoader::ParsingData(const FbxAnimStack* curAnimStack, LArray<LFbxAnimationStack>& outAnim, LFbxLoadContext context)
	{
		LFbxAnimationStack newStackClip(curAnimStack->GetName());
		LUnorderedMap<LString, fbxsdk::FbxNode*> allNodeOut;
		FindAllAnimateNode(context.lScene->GetRootNode(), curAnimStack, allNodeOut);
		FbxTimeSpan AnimStackTimeSpan = GetAnimationTimeSpan(allNodeOut, curAnimStack);
		context.lScene->SetCurrentAnimationStack(const_cast<FbxAnimStack*>(curAnimStack));
		int32_t reSampleRate = 60;
		FbxTime timeIncrement = 0;
		timeIncrement.SetSecondDouble(1.0 / ((double)(reSampleRate)));
		const FbxTime TimeComparisonThreshold = 0.001;
		//将所有层的动画重采样塌陷到第一层
		const_cast<FbxAnimStack*>(curAnimStack)->BakeLayers(context.lScene->GetAnimationEvaluator(), AnimStackTimeSpan.GetStart(), AnimStackTimeSpan.GetStop(), timeIncrement);
		int32_t AnimStackLayerCount = curAnimStack->GetMemberCount();
		assert(AnimStackLayerCount == 1);
		FbxAnimLayer* baseAnimLayer = (FbxAnimLayer*)curAnimStack->GetMember(0);
		for (auto& eachAnimNode : allNodeOut)
		{
			LSharedPtr<LFbxTemplateCurve<FbxDouble3>> newCurveTranslation = MakeShared<LFbxTemplateCurve<FbxDouble3>>(eachAnimNode.first, LFbxAnimationCurveType::NODE_TRANSLATION);
			LSharedPtr<LFbxTemplateCurve<FbxDouble3>> newCurveRotation = MakeShared<LFbxTemplateCurve<FbxDouble3>>(eachAnimNode.first, LFbxAnimationCurveType::NODE_ROTATION);
			LSharedPtr<LFbxTemplateCurve<FbxDouble3>> newCurveScaling = MakeShared<LFbxTemplateCurve<FbxDouble3>>(eachAnimNode.first, LFbxAnimationCurveType::NODE_SCALING);
			LArray<fbxsdk::FbxAnimCurve*> newCurvesTranslation;
			LArray<fbxsdk::FbxAnimCurve*> newCurvesRotation;
			LArray<fbxsdk::FbxAnimCurve*> newCurvesScale;
			newCurvesTranslation.resize(3);
			newCurvesRotation.resize(3);
			newCurvesScale.resize(3);
			newCurvesTranslation[0] = eachAnimNode.second->LclTranslation.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			newCurvesTranslation[1] = eachAnimNode.second->LclTranslation.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			newCurvesTranslation[2] = eachAnimNode.second->LclTranslation.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
			
			newCurvesRotation[0] = eachAnimNode.second->LclRotation.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			newCurvesRotation[1] = eachAnimNode.second->LclRotation.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			newCurvesRotation[2] = eachAnimNode.second->LclRotation.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
			
			newCurvesScale[0] = eachAnimNode.second->LclScaling.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			newCurvesScale[1] = eachAnimNode.second->LclScaling.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			newCurvesScale[2] = eachAnimNode.second->LclScaling.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
			if (newCurvesTranslation[0] != nullptr)
			{
				SampleKeyValue(newCurvesTranslation, newCurveTranslation);
				newStackClip.AddCurve(newCurveTranslation);
			}
			if (newCurvesRotation[0] != nullptr)
			{
				SampleKeyValue(newCurvesRotation, newCurveRotation);
				newStackClip.AddCurve(newCurveRotation);
			}
			if (newCurvesScale[0] != nullptr)
			{
				SampleKeyValue(newCurvesScale, newCurveScaling);
				newStackClip.AddCurve(newCurveScaling);
			}
		}
		outAnim.push_back(newStackClip);
	}

	void LFbxAnimationLoader::SampleKeyValue(LArray<fbxsdk::FbxAnimCurve*> newCurves, LSharedPtr<LFbxTemplateCurve<FbxDouble3>>& curveOut)
	{
		assert(newCurves[0]->KeyGetCount() == newCurves[1]->KeyGetCount());
		assert(newCurves[1]->KeyGetCount() == newCurves[2]->KeyGetCount());
		for (size_t i = 0; i < newCurves[0]->KeyGetCount(); ++i)
		{
			fbxsdk::FbxAnimCurveKey keyValueX = newCurves[0]->KeyGet(i);
			fbxsdk::FbxAnimCurveKey keyValueY = newCurves[1]->KeyGet(i);
			fbxsdk::FbxAnimCurveKey keyValueZ = newCurves[2]->KeyGet(i);
			curveOut->AddTimePoint(keyValueX.GetTime());
			FbxDouble3 vectorData(keyValueX.GetValue(), keyValueY.GetValue(), keyValueZ.GetValue());
			curveOut->AddValue(vectorData);
		}
	}
}