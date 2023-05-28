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

		void ConvertAnimationAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix);
	};

	class ASSET_IMPORT_API LImportNodeSkeletonAnimation : public LImportNodeAnimationBase
	{
		LArray<LSharedPtr<BoneAnimationCurve>> mBoneAnimations;

		LUnorderedMap<LString, size_t> mBoneNameRef;
	public:
		LImportNodeSkeletonAnimation(const size_t index);

		void AddCurveTranslationToNode(const LString &nodeName, float time, LVector3f value);

		void AddCurveRotationToNode(const LString &nodeName, float time, LQuaternion value);

		void AddCurveScaleToNode(const LString &nodeName, float time, LVector3f value);
	private:

		void ConvertAnimationAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) override;

		BoneAnimationCurve* GetOrCreateAnimationCurveByName(const LString &nodeName);
	};
}

