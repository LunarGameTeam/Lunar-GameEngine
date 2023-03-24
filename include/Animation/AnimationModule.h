#pragma once
#include "Core/Framework/Module.h"
#include "Animation/AnimationConfig.h"
#include "Animation/SkeletonAnimation/SkeletalAnimInstance.h"
namespace luna::animation
{
	class ANIMATION_API AnimationModule : public LModule
	{
		RegisterTypeEmbedd(AnimationModule, LModule)
	public:
		AnimationModule();
		void OnIMGUI() override;
	public:

		bool OnLoad() override;
		bool OnInit() override;
		bool OnShutdown() override;

		void Tick(float deltaTime) override;
		void RenderTick(float delta_time) override;

		SkeletalAnimInstance* CreateAnimationInstance(const LString& animAssetName);
		void FreeAnimationInstance(size_t instanceId);
		LQueue<size_t> mEmptyIndex;
	private:
		LUnorderedMap<size_t, TPPtr<SkeletalAnimInstance>> mAnimationInstances;

	};
}