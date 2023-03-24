#include "Animation/AnimationModule.h"


ANIMATION_API luna::animation::AnimationModule* luna::sAnimationModule = nullptr;

namespace luna::animation
{
	RegisterTypeEmbedd_Imp(AnimationModule)
	{
		cls->Ctor<AnimationModule>();
		cls->Binding<Self>();
		BindingModule::Get("luna")->AddType(cls);

	}
	AnimationModule::AnimationModule()
	{
		sAnimationModule = this;
	}

	void AnimationModule::OnIMGUI()
	{
	}

	bool AnimationModule::OnShutdown()
	{
		return true;
	}

	bool AnimationModule::OnLoad()
	{
		return true;
	}

	bool AnimationModule::OnInit()
	{
		return true;
	}


	void AnimationModule::Tick(float deltaTime)
	{
		for(auto &eachData : mAnimationInstances)
		{
			eachData.second->UpdateAnimation(deltaTime);
		}
	}

	void AnimationModule::RenderTick(float delta_time)
	{
	}

	SkeletalAnimInstance* AnimationModule::CreateAnimationInstance(
		const LString& animAssetName)
	{
		size_t newIndex = mAnimationInstances.size();
		if (!mEmptyIndex.empty())
		{
			newIndex = mEmptyIndex.front();
			mEmptyIndex.pop();
		}
		mAnimationInstances.emplace(newIndex, this); 
		mAnimationInstances[newIndex].SetPtr(TCreateObject<SkeletalAnimInstance>());
		return mAnimationInstances[newIndex].Get();
	}

	void AnimationModule::FreeAnimationInstance(size_t instanceId)
	{
		auto nowInstance = mAnimationInstances.find(instanceId);
		if(nowInstance != mAnimationInstances.end())
		{
			mEmptyIndex.push(nowInstance->first);
			mAnimationInstances.erase(nowInstance);
		}
	}


}

