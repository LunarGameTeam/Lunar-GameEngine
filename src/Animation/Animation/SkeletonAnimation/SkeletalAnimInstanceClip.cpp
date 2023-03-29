#include "Animation/SkeletonAnimation/SkeletalAnimInstanceClip.h"
namespace luna::animation
{
	RegisterTypeEmbedd_Imp(SkeletalAnimInstanceClip)
	{
		cls->Ctor<SkeletalAnimInstanceClip>();

		cls->BindingProperty< &Self::mAnimationClipAsset>("animation_clip_asset")
			.Serialize();

		cls->Binding<SkeletalAnimInstanceClip>();
		BindingModule::Get("luna")->AddType(cls);
	}

	SkeletalAnimInstanceClip::SkeletalAnimInstanceClip()
	{
		mAnimationType = SkeletonAnimationType::AnimationClipInstance;
	}

	void SkeletalAnimInstanceClip::SetAssetValue(SharedPtr<AnimationClipAsset> animationClipAsset)
	{
		mAnimationClipAsset = animationClipAsset;
		mSkeletonAsset = animationClipAsset->GetSkeleton();
	}

	void SkeletalAnimInstanceClip::UpdateAnimationImpl(float deltaTime)
	{

	}
}
