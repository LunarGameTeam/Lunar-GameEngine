#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxAnimationLoader.h"
using namespace fbxsdk;
namespace luna::lfbx
{
	FbxTimeSpan LFbxAnimationLoader::GetAnimationTimeSpan(const LUnorderedMap<LString, fbxsdk::FbxNode*> &allNodeOut, const fbxsdk::FbxAnimStack* AnimStack)
	{
		//这里暂时不考虑动画轨道中存在废尾部数据需要裁剪的情况
		FbxTimeSpan AnimTimeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
		AnimTimeSpan = AnimStack->GetLocalTimeSpan();

		return AnimTimeSpan;
	}
	
	bool LFbxAnimationLoader::CheckNodeHasTransformAnimation(fbxsdk::FbxNode* targetNode, fbxsdk::FbxAnimLayer* animLayer)
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
	
	void LFbxAnimationLoader::GetNodeFromAnimLayer(fbxsdk::FbxNode* parentNode, fbxsdk::FbxAnimLayer* animLayer, LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut)
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

	void LFbxAnimationLoader::FindAllAnimateNode(fbxsdk::FbxNode* rootNode, const fbxsdk::FbxAnimStack* curAnimStack, LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut)
	{
		int32_t AnimStackLayerCount = curAnimStack->GetMemberCount();
		for (int32_t LayerIndex = 0; LayerIndex < AnimStackLayerCount; ++LayerIndex)
		{
			FbxAnimLayer* eachAnimLayer = (FbxAnimLayer*)curAnimStack->GetMember(LayerIndex);
			GetNodeFromAnimLayer(rootNode, eachAnimLayer, allNodeOut);
		}
	}

	void LFbxAnimationLoader::ChangeAnimationLength(
		fbxsdk::FbxAnimCurve* curvePointer,
		fbxsdk::FbxTime &timeStart,
		fbxsdk::FbxTime &timeEnd
		)
	{
		int keyCount = curvePointer->KeyGetCount();
		fbxsdk::FbxTime curveTimeStart = curvePointer->KeyGet(0).GetTime();
		fbxsdk::FbxTime curveTimeEnd = curvePointer->KeyGet(keyCount - 1).GetTime();
		if (curveTimeStart < timeStart)
		{
			timeStart = curveTimeStart;
		}
		if (curveTimeEnd > timeEnd)
		{
			timeEnd = curveTimeEnd;
		}
	}

	void LFbxAnimationLoader::CheckAnimationLength(
		fbxsdk::FbxAnimLayer* animLayer,
		LUnorderedMap<LString, fbxsdk::FbxNode*> &allNodeOut,
		fbxsdk::FbxTime &timeStart,
		fbxsdk::FbxTime &timeEnd
	)
	{
		for (auto& eachAnimNode : allNodeOut)
		{
			fbxsdk::FbxAnimCurve* curvePointer[9] = { nullptr };
			curvePointer[0] = eachAnimNode.second->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			curvePointer[1] = eachAnimNode.second->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			curvePointer[2] = eachAnimNode.second->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);

			curvePointer[3] = eachAnimNode.second->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			curvePointer[4] = eachAnimNode.second->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			curvePointer[5] = eachAnimNode.second->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);

			curvePointer[6] = eachAnimNode.second->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
			curvePointer[7] = eachAnimNode.second->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
			curvePointer[8] = eachAnimNode.second->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
			for (int32_t i = 0; i < 9; ++i)
			{
				if (curvePointer[i] != nullptr)
				{
					ChangeAnimationLength(curvePointer[i], timeStart, timeEnd);
				}
			}
		}
	}

	void LFbxAnimationLoader::GenerateConstantKeyCurve(FbxDouble3 constantValue, LFbxTemplateCurve<FbxDouble3>* curveOut, FbxTime startTime, FbxTime endTime, FbxTime lerpDelta)
	{
		for (FbxTime keyTime = startTime; keyTime <= endTime; keyTime += lerpDelta)
		{
			curveOut->AddTimePoint(keyTime);
			curveOut->AddValue(constantValue);
		}
	}
	
	void LFbxAnimationLoader::ResampleFloat3CurveValue(
		FbxDouble3 defaultValue,
		LArray<fbxsdk::FbxAnimCurve*>& curvesArray,
		LFbxTemplateCurve<FbxDouble3>* newCurveData,
		FbxTime startTime,
		FbxTime endTime,
		FbxTime lerpDelta
	)
	{
		if (curvesArray[0] != nullptr)
		{
			SampleKeyValue(curvesArray, newCurveData, startTime, endTime, lerpDelta);
		}
		else
		{
			GenerateConstantKeyCurve(defaultValue, newCurveData, startTime, endTime, lerpDelta);
		}
	}
	
	void LFbxAnimationLoader::AddFloat3CurveToStack(
		FbxPropertyT<FbxDouble3>& curvesProperty,
		fbxsdk::FbxAnimLayer* baseAnimLayer,
		const LString& nodeName,
		LFbxAnimationCurveType curveType,
		FbxTime startTime,
		FbxTime endTime,
		FbxTime lerpDelta,
		LFbxAnimationStack& animStack
		)
	{
		FbxDouble3 defaultValue = curvesProperty;
		LArray<fbxsdk::FbxAnimCurve*> curFbxCurve;
		curFbxCurve.resize(3);
		curFbxCurve[0] = curvesProperty.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
		curFbxCurve[1] = curvesProperty.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
		curFbxCurve[2] = curvesProperty.GetCurve(baseAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
		LSharedPtr<LFbxTemplateCurve<FbxDouble3>> curCurveKeyData = MakeShared<LFbxTemplateCurve<FbxDouble3>>(nodeName, curveType);
		ResampleFloat3CurveValue(defaultValue, curFbxCurve, curCurveKeyData.get(), startTime, endTime, lerpDelta);
		animStack.AddCurve(curCurveKeyData);
	}

	void LFbxAnimationLoader::ParsingData(const fbxsdk::FbxAnimStack* curAnimStack, LArray<LFbxAnimationStack>& outAnim, LFbxLoadContext context)
	{
		LFbxAnimationStack newStackClip(curAnimStack->GetName());
		LUnorderedMap<LString, fbxsdk::FbxNode*> allNodeOut;
		FindAllAnimateNode(context.lScene->GetRootNode(), curAnimStack, allNodeOut);
		FbxTimeSpan AnimStackTimeSpan = GetAnimationTimeSpan(allNodeOut, curAnimStack);
		context.lScene->SetCurrentAnimationStack(const_cast<fbxsdk::FbxAnimStack*>(curAnimStack));
		int32_t reSampleRate = 60;
		FbxTime timeIncrement = 0;
		timeIncrement.SetSecondDouble(1.0 / ((double)(reSampleRate)));
		FbxTime curveTimeStart = AnimStackTimeSpan.GetStart();
		FbxTime curveTimeEnd = AnimStackTimeSpan.GetStop();
		//将所有层的动画重采样塌陷到第一层
		const_cast<fbxsdk::FbxAnimStack*>(curAnimStack)->BakeLayers(context.lScene->GetAnimationEvaluator(), curveTimeStart, curveTimeEnd, timeIncrement);
		//计算动画的长度信息
		int32_t AnimStackLayerCount = curAnimStack->GetMemberCount();
		assert(AnimStackLayerCount == 1);
		fbxsdk::FbxAnimLayer* baseAnimLayer = (FbxAnimLayer*)curAnimStack->GetMember(0);
		curveTimeStart = curveTimeEnd;
		curveTimeEnd = 0;
		CheckAnimationLength(baseAnimLayer, allNodeOut, curveTimeStart, curveTimeEnd);
		newStackClip.SetAnimMessage(curveTimeStart, curveTimeEnd, 60);
		//开始重采样动画的曲线
		for (auto& eachAnimNode : allNodeOut)
		{
			AddFloat3CurveToStack(
				eachAnimNode.second->LclTranslation,
				baseAnimLayer,
				eachAnimNode.first,
				LFbxAnimationCurveType::NODE_TRANSLATION,
				curveTimeStart,
				curveTimeEnd,
				timeIncrement,
				newStackClip
			);
			AddFloat3CurveToStack(
				eachAnimNode.second->LclRotation,
				baseAnimLayer,
				eachAnimNode.first,
				LFbxAnimationCurveType::NODE_ROTATION,
				curveTimeStart,
				curveTimeEnd,
				timeIncrement,
				newStackClip
			);
			AddFloat3CurveToStack(
				eachAnimNode.second->LclScaling,
				baseAnimLayer,
				eachAnimNode.first,
				LFbxAnimationCurveType::NODE_SCALING,
				curveTimeStart,
				curveTimeEnd,
				timeIncrement,
				newStackClip
			);
		}
		outAnim.push_back(newStackClip);
	}

	void LFbxAnimationLoader::SampleKeyValue(
		LArray<fbxsdk::FbxAnimCurve*> newCurves,
		LFbxTemplateCurve<FbxDouble3>* curveOut,
		FbxTime startTime,
		FbxTime endTime,
		FbxTime lerpDelta
	)
	{
		assert(newCurves[0]->KeyGetCount() == newCurves[1]->KeyGetCount());
		assert(newCurves[1]->KeyGetCount() == newCurves[2]->KeyGetCount());
		for (FbxTime keyTime = startTime; keyTime <= endTime; keyTime += lerpDelta)
		{
			curveOut->AddTimePoint(keyTime);
			float keyValueX = newCurves[0]->Evaluate(keyTime);
			float keyValueY = newCurves[1]->Evaluate(keyTime);
			float keyValueZ = newCurves[2]->Evaluate(keyTime);
			FbxDouble3 vectorData(keyValueX, keyValueY, keyValueZ);
			curveOut->AddValue(vectorData);
		}
	}
}