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

		//Asset资源读入到内存时回调
		virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;

		//Asset资源写入到磁盘时回调	
		virtual void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;

		LAnimLerpType mLerpType;

		uint32_t      mFramePerSec;

		float         mAnimClipLength;

		LArray<AnimClickTrackData> mRawData;

		LArray<LString> mBoneName;

		LUnorderedMap<LString, int32_t> mBoneNameIdRef;

		LArray<float> mKeyTimes;
	private:
	};
}