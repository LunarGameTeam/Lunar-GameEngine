#pragma once
#include "AssetImport/ImportScene/ImportDataSkeletonAnimation.h"
namespace luna::asset
{
	BoneAnimationCurve::BoneAnimationCurve(LString nodeName) :
		mNodeName(nodeName),
		mTranslationCurve(nodeName, LImportAnimationCurveType::NODE_TRANSLATION),
		mRotationCurve(nodeName, LImportAnimationCurveType::NODE_ROTATION),
		mScaleCurve(nodeName, LImportAnimationCurveType::NODE_SCAL)
	{
	};

	void BoneAnimationCurve::AddTranslationKey(float time, LVector3f value)
	{
		mTranslationCurve.AddTimePoint(time);
		mTranslationCurve.AddValue(value);
	}

	void BoneAnimationCurve::AddRotationKey(float time, LQuaternion value)
	{
		mRotationCurve.AddTimePoint(time);
		mRotationCurve.AddValue(value);
	}

	void BoneAnimationCurve::AddScaleKey(float time, LVector3f value)
	{
		mScaleCurve.AddTimePoint(time);
		mScaleCurve.AddValue(value);
	}

	void BoneAnimationCurve::ConvertAnimationAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{	
		//变换loacation数据
		const LArray<LVector3f>& existTranslateValues = mTranslationCurve.GetValues();
		for (size_t i =0;i < existTranslateValues.size(); ++i)
		{
			auto locationData = existTranslateValues[i];
			ConvertPositionByTransform(hasReflectTransform, convertInvMatrix, convertMatrix, locationData);
			mTranslationCurve.SetValue(i, locationData);
		}
		
		//变换scale数据
		const LArray<LVector3f>& existScaleValues = mScaleCurve.GetValues();
		for (size_t i = 0; i < existScaleValues.size(); ++i)
		{
			auto scaleData = existScaleValues[i];
			ConvertScaleByTransform(hasReflectTransform, convertInvMatrix, convertMatrix, scaleData);
			mScaleCurve.SetValue(i, scaleData);
		}

		//变换rotation数据
		const LArray<LQuaternion>& existRotationValues = mRotationCurve.GetValues();
		for (size_t i = 0; i < existRotationValues.size(); ++i)
		{
			auto rotationData = existRotationValues[i];
			ConvertRotationByTransform(hasReflectTransform, convertInvMatrix, convertMatrix, rotationData);
			mRotationCurve.SetValue(i, rotationData);
		}
	}

	LImportNodeSkeletonAnimation::LImportNodeSkeletonAnimation(const size_t index) :LImportNodeAnimationBase(LImportNodeAnimationType::ImportSkeletonAnimation, index)
	{

	}

	void LImportNodeSkeletonAnimation::AddCurveTranslationToNode(const LString &nodeName, float time, LVector3f value)
	{
		BoneAnimationCurve* nodeData = GetOrCreateAnimationCurveByName(nodeName);
		nodeData->AddTranslationKey(time, value);
	}

	void LImportNodeSkeletonAnimation::AddCurveRotationToNode(const LString &nodeName, float time, LQuaternion value)
	{
		BoneAnimationCurve* nodeData = GetOrCreateAnimationCurveByName(nodeName);
		nodeData->AddRotationKey(time, value);
	}

	void LImportNodeSkeletonAnimation::AddCurveScaleToNode(const LString &nodeName, float time, LVector3f value)
	{
		BoneAnimationCurve* nodeData = GetOrCreateAnimationCurveByName(nodeName);
		nodeData->AddScaleKey(time, value);
	}

	BoneAnimationCurve* LImportNodeSkeletonAnimation::GetOrCreateAnimationCurveByName(const LString &nodeName)
	{
		auto existNode = mBoneNameRef.find(nodeName);
		if (existNode != mBoneNameRef.end())
		{
			return mBoneAnimations[existNode->second].get();
		}
		LSharedPtr<BoneAnimationCurve> newCurve = MakeShared<BoneAnimationCurve>(nodeName);
		mBoneNameRef.insert({ nodeName ,mBoneAnimations.size()});
		mBoneAnimations.push_back(newCurve);
		return newCurve.get();
	}

	void LImportNodeSkeletonAnimation::ConvertAnimationAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		for (auto& eachBoneAnim : mBoneAnimations)
		{
			eachBoneAnim->ConvertAnimationAxisAndUnitImpl(hasReflectTransform, convertInvMatrix, convertMatrix);
		}
	}
}

