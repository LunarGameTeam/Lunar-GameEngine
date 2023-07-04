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

		const LString& GetAnimInstanceUniqueName() { return mInstanceUniqueName; }

		void UpdateAnimation(float deltaTime);

		void ApplyAnimation();

		void SetAsset(const Asset* animationAsset);

		void SetSkeleton(const SkeletonAsset* skeletonAsset);

		const Asset* GetAsset()const { return mAnimationAsset; }

		void SetOnUpdateFinishMethod(std::function<void(const LArray<LMatrix4f>& allBoneMatrix)> func)
		{
			onUpdateFinishedFunc = func;
		};
	protected:
		size_t mIndex;

		SkeletonAnimationType mAnimationType;

		const SkeletonAsset* mSkeletonAsset;

		const Asset* mAnimationAsset;

		LString mInstanceUniqueName;
	private:
		virtual void UpdateAnimationImpl(float deltaTime, LArray<LMatrix4f>& allBoneMatrix) { assert(false); }

		virtual void OnAssetChanged(const Asset* animationAsset) { assert(false); }

		std::function<void(const LArray<LMatrix4f>& allBoneMatrix)> onUpdateFinishedFunc;

		LArray<LMatrix4f> poseMatrix;
	};


}
