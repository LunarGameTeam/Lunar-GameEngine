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
	size_t SkeletonBoneSize;
	memcpy(&SkeletonBoneSize, ptr, sizeof(size_t));
	ptr += sizeof(size_t);
	mBoneTree.clear();
	for (size_t id = 0; id < SkeletonBoneSize; ++id)
	{
		LSingleBone newBoneData;
		memcpy(&newBoneData, ptr, sizeof(LSingleBone));
		ptr += sizeof(LSingleBone);
		mSearchIndex.insert({ newBoneData.mBoneName,mBoneTree.size() });
		mBoneTree.push_back(newBoneData);
	}
	OnLoad();
};

void SkeletonAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
{
	size_t globel_size = 0;
	size_t offset = 0;
	globel_size += sizeof(size_t);
	for (size_t id = 0; id < mBoneTree.size(); ++id)
	{
		globel_size += sizeof(LSingleBone);
	}
	data.resize(globel_size);
	byte* dst = data.data();
	size_t SkeletonBoneSize = mBoneTree.size();
	memcpy(dst, &SkeletonBoneSize, sizeof(size_t));
	dst += sizeof(size_t);
	memcpy(dst, mBoneTree.data(), SkeletonBoneSize * sizeof(LSingleBone));
}

}
