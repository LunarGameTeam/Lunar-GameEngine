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

	void SkeletalAnimInstanceBase::SetIndex(size_t index)
	{
		mIndex = index;
	}

	void SkeletalAnimInstanceBase::SetAsset(const Asset* animationAsset)
	{
		mAnimationAsset = animationAsset;
		OnAssetChanged(mAnimationAsset);
	}

	void SkeletalAnimInstanceBase::UpdateAnimation(float deltaTime)
	{
		UpdateAnimationImpl(deltaTime);
	}

	void SkeletalAnimInstanceBase::ApplyAnimation()
	{

	}


}
