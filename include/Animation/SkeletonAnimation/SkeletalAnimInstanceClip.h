#pragma once
#include "Animation/SkeletonAnimation/SkeletalAnimInstanceBase.h"
#include "Animation/Asset/AnimationClipAsset.h"
namespace luna::animation
{
	class ANIMATION_API SkeletalAnimInstanceClip : public SkeletalAnimInstanceBase
	{
		RegisterTypeEmbedd(SkeletalAnimInstanceClip, SkeletalAnimInstanceBase)
	public:
		SkeletalAnimInstanceClip();
	private:
		const AnimationClipAsset* mClipAsset;

		void UpdateAnimationImpl(float deltaTime) override;

		void OnAssetChanged(const Asset* animationAsset) override;
	};
}
