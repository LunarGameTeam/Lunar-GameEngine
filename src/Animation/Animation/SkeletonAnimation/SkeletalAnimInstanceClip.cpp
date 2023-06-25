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

	void SkeletalAnimInstanceClip::UpdateAnimationImpl(float deltaTime)
	{
	}

	void SkeletalAnimInstanceClip::OnAssetChanged(const Asset* animationAsset)
	{
		if (mClipAsset == nullptr)
		{
			mClipAsset = dynamic_cast<const AnimationClipAsset*>(mAnimationAsset);
		}
	}
}
