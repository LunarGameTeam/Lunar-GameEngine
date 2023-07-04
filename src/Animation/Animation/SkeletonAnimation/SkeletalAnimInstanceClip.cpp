#include "Animation/SkeletonAnimation/SkeletalAnimInstanceClip.h"
namespace luna::animation
{
	RegisterTypeEmbedd_Imp(SkeletalAnimInstanceClip)
	{
		cls->Ctor<SkeletalAnimInstanceClip>();
		cls->Binding<SkeletalAnimInstanceClip>();
		BindingModule::Get("luna")->AddType(cls);
	}

	SkeletalAnimInstanceClip::SkeletalAnimInstanceClip():mClipAsset(nullptr)
	{
		mAnimationType = SkeletonAnimationType::AnimationClipInstance;
	}

	void SkeletalAnimInstanceClip::UpdateAnimationImpl(float deltaTime, LArray<LMatrix4f>& allBoneMatrix)
	{
		mCurPlayTime += deltaTime;
		if (mCurPlayTime + 0.001f > mClipAsset->mAnimClipLength)
		{
			if (mIsLoop)
			{
				mCurPlayTime = 0;
			}
			else 
			{
				mCurPlayTime = mClipAsset->mAnimClipLength;
			}
		}
		float animDeltaTime = 1.0f / float(mClipAsset->mFramePerSec);
		uint32_t timeIndexPre = mCurPlayTime / animDeltaTime;
		uint32_t timeIndexNext = std::min(timeIndexPre + 1, (uint32_t)mClipAsset->mKeyTimes.size());
		float lerpDelta = (mCurPlayTime - mClipAsset->mKeyTimes[timeIndexPre]) / animDeltaTime;
		LVector3f posLerp, scaleLerp;
		LQuaternion rotLerp;
		for (uint32_t i = 0; i < mSkeletonAsset->GetBoneCount(); ++i)
		{
			const animation::LSingleBone& boneData = mSkeletonAsset->GetBone(i);
			auto itor = mClipAsset->mBoneNameIdRef.find(boneData.mBoneName);
			if (itor != mClipAsset->mBoneNameIdRef.end())
			{
				const LVector3f& posValuePre = mClipAsset->mRawData[itor->second].mPosKeys[timeIndexPre];
				const LVector3f& posValueEnd = mClipAsset->mRawData[itor->second].mPosKeys[timeIndexNext];
				posLerp = posValuePre * (1.0f - lerpDelta) + lerpDelta * posValueEnd;

				const LQuaternion& rotValuePre = mClipAsset->mRawData[itor->second].mRotKeys[timeIndexPre];
				const LQuaternion& rotValueEnd = mClipAsset->mRawData[itor->second].mRotKeys[timeIndexNext];
				rotLerp = rotValuePre;
				rotLerp = rotLerp.slerp(lerpDelta, rotValueEnd);

				const LVector3f& scaleValuePre = mClipAsset->mRawData[itor->second].mScalKeys[timeIndexPre];
				const LVector3f& scaleValueEnd = mClipAsset->mRawData[itor->second].mScalKeys[timeIndexNext];
				scaleLerp = scaleValuePre * (1.0f - lerpDelta) + lerpDelta * scaleValueEnd;
			}
			else
			{
				posLerp = boneData.mBaseTranslation;
				rotLerp = boneData.mBaseRotation;
				scaleLerp = boneData.mBaseScal;
			}
			LMatrix4f worldSpacePose = LMath::MatrixCompose(posLerp, rotLerp, scaleLerp);
			if (boneData.mParentIndex != uint32_t(-1))
			{
				worldSpacePose = allBoneMatrix[boneData.mParentIndex] * worldSpacePose;
			}
			allBoneMatrix[i] = worldSpacePose;
		}
	}

	void SkeletalAnimInstanceClip::OnAssetChanged(const Asset* animationAsset)
	{
		if (mClipAsset == nullptr)
		{
			mClipAsset = dynamic_cast<const AnimationClipAsset*>(mAnimationAsset);
		}
	}
}
