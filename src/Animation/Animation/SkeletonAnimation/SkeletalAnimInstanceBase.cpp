#include "Animation/SkeletonAnimation/SkeletalAnimInstanceBase.h"
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

	void SkeletalAnimInstanceBase::UpdateAnimation(float deltaTime)
	{
		UpdateAnimationImpl(deltaTime);
	}

}
