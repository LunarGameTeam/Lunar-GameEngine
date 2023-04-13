#include "Animation/AnimationModule.h"
#include "Animation/SkeletonAnimation/SkeletalAnimInstanceClip.h"
#include "Core/Asset/AssetModule.h"

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
		mNeedTick = true;
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

	SkeletalAnimInstanceBase* AnimationModule::CreateAnimationInstance(
		const LString& animAssetName)
	{
		size_t newIndex = mAnimationInstances.size();
		if (!mEmptyIndex.empty())
		{
			newIndex = mEmptyIndex.front();
			mEmptyIndex.pop();
		}
		mAnimationInstances.emplace(newIndex, this);
		SkeletalAnimInstanceClip* newInstance = TCreateObject<SkeletalAnimInstanceClip>();
		LSharedPtr<AnimationClipAsset> animationAsset = sAssetModule->LoadAsset<AnimationClipAsset>(animAssetName);
		newInstance->SetAssetValue(animationAsset);
		newInstance->SetIndex(newIndex);
		mAnimationInstances[newIndex].SetPtr(newInstance);
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

