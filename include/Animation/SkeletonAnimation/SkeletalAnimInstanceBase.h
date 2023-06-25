#pragma once
#include "Core/CoreMin.h"
#include "Core/Object/SharedObject.h"
#include "Animation/AnimationConfig.h"
#include "Animation/Asset/SkeletonAsset.h"
namespace luna::animation
{
	class AnimationClipAsset;

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

		size_t GetAnimIndex() { return mIndex; };

		void UpdateAnimation(float deltaTime);

		void ApplyAnimation();

		void SetAsset(const Asset* animationAsset);

		void SetSkeleton(const SkeletonAsset* skeletonAsset) { mSkeletonAsset = skeletonAsset; }

		void SetRenderObject(const uint64_t roData) { mRO = roData; }
	protected:
		size_t mIndex;

		SkeletonAnimationType mAnimationType;

		const SkeletonAsset* mSkeletonAsset;

		const Asset* mAnimationAsset;

		uint64_t mRO;

	private:
		virtual void UpdateAnimationImpl(float deltaTime) { assert(false); }

		virtual void OnAssetChanged(const Asset* animationAsset) { assert(false); }
	};


}
