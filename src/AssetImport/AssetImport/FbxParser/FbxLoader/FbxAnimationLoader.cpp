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
		for (auto& eachAnimNode : allNodeOut)
		{
			std::shared_ptr<LFbxTemplateCurve<FbxVector4>> newCurveTranslation = std::make_shared<LFbxTemplateCurve<FbxVector4>>(eachAnimNode.first, LFbxAnimationCurveType::NODE_TRANSLATION);
			std::shared_ptr<LFbxTemplateCurve<FbxVector4>> newCurveScaling = std::make_shared<LFbxTemplateCurve<FbxVector4>>(eachAnimNode.first, LFbxAnimationCurveType::NODE_SCALING);
			std::shared_ptr<LFbxTemplateCurve<FbxQuaternion>> newCurveRotation = std::make_shared<LFbxTemplateCurve<FbxQuaternion>>(eachAnimNode.first, LFbxAnimationCurveType::NODE_ROTATION);
			for (FbxTime curTime = AnimStackTimeSpan.GetStart(); curTime < (AnimStackTimeSpan.GetStop() + TimeComparisonThreshold); curTime += timeIncrement)
			{
				FbxAMatrix nodeLocalMatrix = eachAnimNode.second->EvaluateLocalTransform(curTime);
				FbxVector4 newLocalT = nodeLocalMatrix.GetT();
				FbxVector4 newLocalS = nodeLocalMatrix.GetS();
				FbxQuaternion newLocalQ = nodeLocalMatrix.GetQ();
				newCurveTranslation->AddValue(newLocalT);
				newCurveScaling->AddValue(newLocalS);
				newCurveRotation->AddValue(newLocalQ);
			}
			newStackClip.AddCurve(newCurveTranslation);
			newStackClip.AddCurve(newCurveRotation);
			newStackClip.AddCurve(newCurveScaling);
		}
		outAnim.push_back(newStackClip);
	}
}