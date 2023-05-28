#pragma once
#include "Core/CoreMin.h"
#include "Core/Object/SharedObject.h"
#include "Animation/AnimationConfig.h"
#include "Animation/Asset/SkeletonAsset.h"
namespace luna::animation
{
	enum SkeletonAnimationType
	{
		AnimationClipInstance = 0,
		AnimationMontageInstance,
		AnimationGraphInstance
	};

	class ANIMATION_API SkeletalAnimInstanceBase : public LObject
	{
		RegisterTypeEmbedd(SkeletalAnimInstanceBase, LObject)

	public:
		SkeletalAnimInstanceBase();

		void SetIndex(size_t index);

		void UpdateAnimation(float deltaTime);

		void ApplyAnimation();
	protected:
		size_t mIndex;

		SkeletonAnimationType mAnimationType;

		SharedPtr<SkeletonAsset> mSkeletonAsset;

	private:
		virtual void UpdateAnimationImpl(float deltaTime) { assert(false); };
	};
}
