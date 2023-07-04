#include "Animation/SkeletonAnimation/SkeletalAnimInstanceBase.h"
#include "Animation/Asset/AnimationClipAsset.h"
namespace luna::animation
{
	RegisterTypeEmbedd_Imp(SkeletalAnimInstanceBase)
	{
		cls->Ctor<SkeletalAnimInstanceBase>();
		cls->Binding<SkeletalAnimInstanceBase>();
		BindingModule::Get("luna")->AddType(cls);
	}

	SkeletalAnimInstanceBase::SkeletalAnimInstanceBase()
	{
	}

	void SkeletalAnimInstanceBase::SetSkeleton(const SkeletonAsset* skeletonAsset)
	{ 
		mSkeletonAsset = skeletonAsset;
		poseMatrix.resize(mSkeletonAsset->GetBoneCount());
	}

	void SkeletalAnimInstanceBase::SetIndex(size_t index)
	{
		mIndex = index;
		mInstanceUniqueName = "AnimationInstance::" + std::to_string(mIndex);
	}

	void SkeletalAnimInstanceBase::SetAsset(const Asset* animationAsset)
	{
		mAnimationAsset = animationAsset;
		OnAssetChanged(mAnimationAsset);
	}

	void SkeletalAnimInstanceBase::UpdateAnimation(float deltaTime)
	{
		UpdateAnimationImpl(deltaTime, poseMatrix);
		onUpdateFinishedFunc(poseMatrix);
	}

	void SkeletalAnimInstanceBase::ApplyAnimation()
	{

	}


}
