#include "Animation/Asset/AnimationClipAsset.h"
#include "Core/Asset/AssetModule.h"

namespace luna::animation
{

	RegisterTypeEmbedd_Imp(AnimationClipAsset)
	{
		cls->Binding<Self>();
		cls->BindingProperty<&Self::mSkeleton>("shader")
			.Serialize();
		BindingModule::Get("luna")->AddType(cls);
		cls->Ctor<AnimationClipAsset>();
	};

	void AnimationClipAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
	{
		const byte* data_header = file->GetData().data();
		size_t offset = 0;
		const byte* ptr = data_header;
		char SkeletonPathName[64] = {};
		memcpy(&SkeletonPathName, ptr, 64 * sizeof(char));
		ptr += 64 * sizeof(char);
		mSkeletonPath = SkeletonPathName;
		mSkeleton = sAssetModule->LoadAsset<SkeletonAsset>(mSkeletonPath);
		memcpy(&mLerpType, ptr, sizeof(mLerpType));
		ptr += sizeof(mLerpType);
		memcpy(&mFramePerSec, ptr, sizeof(mFramePerSec));
		ptr += sizeof(mFramePerSec);
		memcpy(&mAnimClipLength, ptr, sizeof(mAnimClipLength));
		ptr += sizeof(mAnimClipLength);
		uint32_t AnimTrackSize = 0;
		memcpy(&AnimTrackSize, ptr, sizeof(AnimTrackSize));
		ptr += sizeof(AnimTrackSize);
		for (uint32_t id = 0; id < AnimTrackSize; ++id)
		{
			AnimClickTrackData newTrackata;
			memcpy(&AnimTrackSize, ptr, sizeof(newTrackata));
			ptr += sizeof(newTrackata);
			mRawData.push_back(newTrackata);
		}
		for (uint32_t id = 0; id < AnimTrackSize; ++id)
		{
			char boneNameTrack[64] = {};
			memcpy(&boneNameTrack, ptr, 64 * sizeof(char));
			mBoneName.push_back(boneNameTrack);
			ptr += 64 * sizeof(char);
			mBoneNameIdRef.insert({ boneNameTrack ,id });
		}
		OnLoad();
	};

	void AnimationClipAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
	{
		size_t globel_size = 0;
		size_t offset = 0;
		byte* dst = data.data();
		char SkeletonPathName[64] = {};
		strcpy(SkeletonPathName, mSkeletonPath.c_str());
		memcpy(dst, SkeletonPathName, 64 * sizeof(char));
		dst += 64 * sizeof(char);
		mSkeletonPath = SkeletonPathName;
		memcpy(dst,&mLerpType, sizeof(mLerpType));
		dst += sizeof(mLerpType);
		memcpy(dst, &mFramePerSec, sizeof(mFramePerSec));
		dst += sizeof(mFramePerSec);
		memcpy(dst, &mAnimClipLength, sizeof(mAnimClipLength));
		dst += sizeof(mAnimClipLength);
		uint32_t AnimTrackSize = mRawData.size();
		memcpy(dst ,&AnimTrackSize, sizeof(AnimTrackSize));
		dst += sizeof(AnimTrackSize);
		memcpy(dst, mRawData.data(), AnimTrackSize * sizeof(AnimClickTrackData));
		dst += AnimTrackSize * sizeof(AnimClickTrackData);
		for (uint32_t id = 0; id < AnimTrackSize; ++id)
		{
			char boneNameTrack[64] = {};
			strcpy(boneNameTrack, mBoneName[id].c_str());
			memcpy(dst, boneNameTrack, 64 * sizeof(char));
			dst += 64 * sizeof(char);
		}
	}

}
