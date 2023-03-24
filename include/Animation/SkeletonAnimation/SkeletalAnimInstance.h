#pragma once
#include "Core/CoreMin.h"
#include "Core/Object/SharedObject.h"
#include "Animation/AnimationConfig.h"
namespace luna::animation
{
	enum SkeletonAnimationType
	{
		AnimationClipInstance = 0,
		AnimationMontageInstance,
		AnimationGraphInstance
	};

	class ANIMATION_API SkeletalAnimInstance : public LObject
	{
		RegisterTypeEmbedd(SkeletalAnimInstance, LObject)
	public:
		SkeletalAnimInstance();
		void SetIndex(size_t index);
		void UpdateAnimation(float deltaTime);
	private:
		size_t mIndex;
		SkeletonAnimationType mAnimationType;
	};
}
