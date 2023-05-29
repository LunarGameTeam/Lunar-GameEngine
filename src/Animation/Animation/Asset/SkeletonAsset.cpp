#include "Animation/Asset/SkeletonAsset.h"
namespace luna::animation
{

RegisterTypeEmbedd_Imp(SkeletonAsset)
{
	cls->Ctor<SkeletonAsset>();
	BindingModule::Get("luna")->AddType(cls);
}

SkeletonAsset::SkeletonAsset()
{
}

SkeletonAsset::~SkeletonAsset()
{
}


void SkeletonAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	const byte* data_header = file->GetData().data();
	size_t offset = 0;
	const byte* ptr = data_header;
	size_t SkeletonBoneSize = 0;
	memcpy(&SkeletonBoneSize, ptr, sizeof(size_t));
	ptr += sizeof(size_t);
	mBoneTree.clear();
	for (size_t id = 0; id < SkeletonBoneSize; ++id)
	{
		LSingleBone newBoneData;
		size_t curBoneNameLength = 0;
		memcpy(&curBoneNameLength, ptr, sizeof(size_t));
		ptr += sizeof(size_t);

		char* namePtr = new char[curBoneNameLength + 1];
		namePtr[curBoneNameLength] = '\0';
		memcpy(namePtr, ptr, curBoneNameLength * sizeof(char));
		newBoneData.mBoneName = namePtr;
		ptr += curBoneNameLength * sizeof(char);
		delete[] namePtr;

		memcpy(&newBoneData.mBaseTranslation, ptr, sizeof(newBoneData.mBaseTranslation));
		ptr += sizeof(newBoneData.mBaseTranslation);
		memcpy(&newBoneData.mBaseRotation, ptr, sizeof(newBoneData.mBaseRotation));
		ptr += sizeof(newBoneData.mBaseRotation);
		memcpy(&newBoneData.mBaseScal, ptr, sizeof(newBoneData.mBaseScal));
		ptr += sizeof(newBoneData.mBaseScal);
		memcpy(&newBoneData.mParentIndex, ptr, sizeof(newBoneData.mParentIndex));
		ptr += sizeof(newBoneData.mParentIndex);

		mSearchIndex.insert({ newBoneData.mBoneName,(int32_t)mBoneTree.size() });
		mBoneTree.push_back(newBoneData);
	}
	OnLoad();
};

void SkeletonAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
{
	size_t SkeletonBoneSize = mBoneTree.size();
	CopyPointToByteArray(&SkeletonBoneSize,sizeof(size_t),data);
	for (int32_t skeletonBoneId = 0; skeletonBoneId < SkeletonBoneSize; ++skeletonBoneId)
	{
		LSingleBone& curBoneData = mBoneTree[skeletonBoneId];
		size_t curBoneNameLength = curBoneData.mBoneName.Length();
		CopyPointToByteArray(&curBoneNameLength, sizeof(size_t), data);
		CopyPointToByteArray(curBoneData.mBoneName.c_str(), curBoneNameLength * sizeof(char), data);
		CopyPointToByteArray(&curBoneData.mBaseTranslation,sizeof(curBoneData.mBaseTranslation),data);
		CopyPointToByteArray(&curBoneData.mBaseRotation, sizeof(curBoneData.mBaseRotation), data);
		CopyPointToByteArray(&curBoneData.mBaseScal, sizeof(curBoneData.mBaseScal), data);
		CopyPointToByteArray(&curBoneData.mParentIndex, sizeof(curBoneData.mParentIndex), data);
	}
}

void SkeletonAsset::AddBoneToTree(const LSingleBone& boneData)
{
	mSearchIndex.insert({ boneData.mBoneName,(int32_t)mBoneTree.size() });
	mBoneTree.push_back(boneData);

};

}
