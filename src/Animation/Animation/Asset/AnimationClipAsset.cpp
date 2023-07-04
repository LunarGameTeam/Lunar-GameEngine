#include "Animation/Asset/AnimationClipAsset.h"
#include "Core/Asset/AssetModule.h"

namespace luna::animation
{

	RegisterTypeEmbedd_Imp(AnimationClipAsset)
	{
		cls->Binding<Self>();
		BindingModule::Get("luna")->AddType(cls);
		cls->Ctor<AnimationClipAsset>();
	};

	void AnimationClipAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
	{
		const byte* data_header = file->GetData().data();
		size_t offset = 0;
		const byte* ptr = data_header;
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
			//曲线名称
			size_t curBoneNameLength = 0;
			memcpy(&curBoneNameLength, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			char* namePtr = new char[curBoneNameLength + 1];
			namePtr[curBoneNameLength] = '\0';
			memcpy(namePtr, ptr, curBoneNameLength * sizeof(char));
			mBoneName.push_back(namePtr);
			mBoneNameIdRef.insert({ namePtr ,id });
			ptr += curBoneNameLength * sizeof(char);
			delete[] namePtr;

			//平移曲线
			mRawData.emplace_back();
			size_t posKeySize = 0;
			memcpy(&posKeySize, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			mRawData.back().mPosKeys.resize(posKeySize);
			memcpy(mRawData.back().mPosKeys.data(), ptr, posKeySize * sizeof(LVector3f));
			ptr += posKeySize * sizeof(LVector3f);

			//旋转曲线
			size_t rotKeySize = 0;
			memcpy(&rotKeySize, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			mRawData.back().mRotKeys.resize(rotKeySize);
			memcpy(mRawData.back().mRotKeys.data(), ptr, rotKeySize * sizeof(LQuaternion));
			ptr += posKeySize * sizeof(LQuaternion);

			//缩放曲线
			size_t scaleKeySize = 0;
			memcpy(&scaleKeySize, ptr, sizeof(size_t));
			ptr += sizeof(size_t);
			mRawData.back().mScalKeys.resize(scaleKeySize);
			memcpy(mRawData.back().mScalKeys.data(), ptr, scaleKeySize * sizeof(LVector3f));
			ptr += scaleKeySize * sizeof(LVector3f);
		}
		size_t timeKeySize = 0;
		memcpy(&timeKeySize, ptr, sizeof(size_t));
		ptr += sizeof(size_t);
		mKeyTimes.resize(timeKeySize);
		memcpy(mKeyTimes.data(), ptr, timeKeySize * sizeof(float));
		ptr += timeKeySize * sizeof(float);
		OnLoad();
	};

	void AnimationClipAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
	{
		size_t globel_size = 0;
		size_t offset = 0;
		CopyPointToByteArray(&mLerpType, sizeof(mLerpType), data);
		CopyPointToByteArray(&mFramePerSec, sizeof(mFramePerSec),data);
		CopyPointToByteArray(&mAnimClipLength, sizeof(mAnimClipLength), data);
		uint32_t AnimTrackSize = mRawData.size();
		CopyPointToByteArray(&AnimTrackSize, sizeof(AnimTrackSize), data);
		for (uint32_t id = 0; id < AnimTrackSize; ++id)
		{
			//曲线名称
			size_t curBoneNameLength = mBoneName[id].Length();
			CopyPointToByteArray(&curBoneNameLength, sizeof(size_t), data);
			CopyPointToByteArray(mBoneName[id].c_str(), curBoneNameLength * sizeof(char), data);
			
			//平移曲线
			size_t posKeySize = mRawData[id].mPosKeys.size();
			CopyPointToByteArray(&posKeySize, sizeof(size_t), data);
			CopyPointToByteArray(mRawData[id].mPosKeys.data(), posKeySize * sizeof(LVector3f), data);

			//旋转曲线
			size_t rotKeySize = mRawData[id].mRotKeys.size();
			CopyPointToByteArray(&rotKeySize, sizeof(size_t), data);
			CopyPointToByteArray(mRawData[id].mRotKeys.data(), rotKeySize * sizeof(LQuaternion), data);
			
			//缩放曲线
			size_t scaleKeySize = mRawData[id].mScalKeys.size();
			CopyPointToByteArray(&scaleKeySize, sizeof(size_t), data);
			CopyPointToByteArray(mRawData[id].mScalKeys.data(), scaleKeySize * sizeof(LVector3f), data);
		}
		size_t timeKeySize = mKeyTimes.size();
		CopyPointToByteArray(&timeKeySize, sizeof(size_t), data);
		CopyPointToByteArray(mKeyTimes.data(), timeKeySize * sizeof(float), data);
	}

}
