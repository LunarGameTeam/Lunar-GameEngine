#pragma once
#include "Animation/SkeletonAnimation/SkeletalAnimInstanceBase.h"
#include "Animation/Asset/AnimationClipAsset.h"
namespace luna::animation
{
	class ANIMATION_API SkeletalAnimInstanceClip : public SkeletalAnimInstanceBase
	{
		RegisterTypeEmbedd(SkeletalAnimInstanceClip, LObject)

	public:
		SkeletalAnimInstanceClip();

		void SetAssetValue(SharedPtr<AnimationClipAsset> animationClipAsset);

	private:
		SharedPtr<AnimationClipAsset> mAnimationClipAsset;

		void UpdateAnimationImpl(float deltaTime) override;
	};
}
