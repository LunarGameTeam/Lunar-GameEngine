#pragma once
#include "Core/Foundation/Container.h"
#include "Core/Object/BaseObject.h"
#include "Core/Asset/BinaryAsset.h"
#include "Animation/AnimationConfig.h"
#include "Animation/Asset/SkeletonAsset.h"
namespace luna::animation
{
	enum class LAnimLerpType : uint8_t
	{
		LerpLinear,
		LerpStep
	};
	struct AnimClickTrackData
	{
		LArray<LVector3f>   mPosKeys;
		LArray<LQuaternion> mRotKeys;
		LArray<LVector3f>   mScalKeys;
	};
	class ANIMATION_API AnimationClipAsset : public Asset
	{
		RegisterTypeEmbedd(AnimationClipAsset, Asset)
	public:
		AnimationClipAsset()
		{
		}

		//Asset��Դ���뵽�ڴ�ʱ�ص�
		virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;

		//Asset��Դд�뵽����ʱ�ص�	
		virtual void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;

		SharedPtr<SkeletonAsset> GetSkeleton() { return mSkeleton; };
	private:
		LString mSkeletonPath;

		LAnimLerpType mLerpType;

		uint32_t      mFramePerSec;

		float         mAnimClipLength;

		LArray<AnimClickTrackData> mRawData;

		LArray<LString> mBoneName;

		LUnorderedMap<LString,int32_t> mBoneNameIdRef;

		SharedPtr<SkeletonAsset> mSkeleton;
	};
}