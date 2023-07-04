#pragma once
#include "Animation/SkeletonAnimation/SkeletalAnimInstanceBase.h"
#include "Animation/Asset/AnimationClipAsset.h"
namespace luna::render
{
	class RenderScene;
}
namespace luna::animation
{
	class ANIMATION_API SkeletalAnimInstanceClip : public SkeletalAnimInstanceBase
	{
		RegisterTypeEmbedd(SkeletalAnimInstanceClip, SkeletalAnimInstanceBase)
	public:
		SkeletalAnimInstanceClip();
	private:
		bool mIsLoop = true;

		float mCurPlayTime = 0.0f;

		const AnimationClipAsset* mClipAsset;

		luna::render::RenderScene* mRenderSceneProxcy;

		void UpdateAnimationImpl(float deltaTime, LArray<LMatrix4f>& allBoneMatrix) override;

		void OnAssetChanged(const Asset* animationAsset) override;
	};
}
