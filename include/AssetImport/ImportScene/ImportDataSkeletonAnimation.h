#pragma once

#include "AssetImport/ImportScene/ImportDataCommon.h"

namespace luna::asset
{
	class BoneAnimationCurve
	{
		LString mNodeName;

		LImportAnimationTemplateCurve<LVector3f> mTranslationCurve;

		LImportAnimationTemplateCurve<LQuaternion> mRotationCurve;

		LImportAnimationTemplateCurve<LVector3f> mScaleCurve;
	public:
		BoneAnimationCurve(LString nodeName);

		void AddTranslationKey(float time, LVector3f value);

		void AddRotationKey(float time, LQuaternion value);

		void AddScaleKey(float time, LVector3f value);

		const LImportAnimationTemplateCurve<LVector3f>& GetTranslateCurve()const { return mTranslationCurve; }

		const LImportAnimationTemplateCurve<LQuaternion>& GetRotationCurve()const { return mRotationCurve; }

		const LImportAnimationTemplateCurve<LVector3f>& GetScaleCurve()const { return mScaleCurve; }

		const LString& GetBoneName() const{ return mNodeName; }

		void ConvertAnimationAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix);
	};

	class ASSET_IMPORT_API LImportNodeSkeletonAnimation : public LImportNodeAnimationBase
	{
		int32_t mSkeletonIndex;

		LArray<LSharedPtr<BoneAnimationCurve>> mBoneAnimations;

		LUnorderedMap<LString, size_t> mBoneNameRef;

		float mAnimLength = 0;

		uint32_t mAnimFramePerSecond = 0;
	public:
		LImportNodeSkeletonAnimation(const size_t index);

		void AddCurveTranslationToNode(const LString &nodeName, float time, LVector3f value);

		void AddCurveRotationToNode(const LString &nodeName, float time, LQuaternion value);

		void AddCurveScaleToNode(const LString &nodeName, float time, LVector3f value);

		void SetSkeletonIndex(int32_t skeletonIndex) { mSkeletonIndex = skeletonIndex; }

		void SetAnimationLength(float animLength) { mAnimLength = animLength; }

		float GetAnimationLength() const{return mAnimLength; }

		void SetAnimationFps(uint32_t animDeltatime) { mAnimFramePerSecond = animDeltatime; }

		float GetAnimationFps() const { return mAnimFramePerSecond; }

		int32_t GetSkeletonIndex() const{ return mSkeletonIndex; }

		size_t GetAnimationCurveCount() const { return mBoneAnimations.size(); }

		const BoneAnimationCurve* GetAnimationCurveByIndex(size_t index) const { return mBoneAnimations[index].get(); }
	private:

		void ConvertAnimationAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) override;

		BoneAnimationCurve* GetOrCreateAnimationCurveByName(const LString &nodeName);
	};
}

