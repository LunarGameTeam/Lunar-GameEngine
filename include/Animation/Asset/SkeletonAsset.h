#pragma once
#include "Core/CoreMin.h"
#include "Animation/AnimationConfig.h"
#include "Core/Asset/Asset.h"


namespace luna::animation
{

	struct LSingleBone
	{
		LString mBoneName;
		uint32_t mParentIndex;
		LVector3f mBaseTranslation;
		LQuaternion mBaseRotation;
		LVector3f mBaseScal;
	};

	class ANIMATION_API SkeletonAsset : public Asset
	{
		RegisterTypeEmbedd(SkeletonAsset, Asset)

	public:
		SkeletonAsset();
		~SkeletonAsset() override;
		//Asset资源读入到内存时回调
		virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
		//Asset资源写入到磁盘时回调	
		virtual void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;

		void AddBoneToTree(const LSingleBone& boneData);

		const LUnorderedMap<LString, int32_t>& GetSearchIndex() { return mSearchIndex; }

		size_t GetBoneCount() const{ return mBoneTree.size(); }

		const LSingleBone& GetBone(size_t index)const { return mBoneTree[index]; };
	private:
		LArray<LSingleBone> mBoneTree;
		LUnorderedMap<LString, int32_t> mSearchIndex;
	};
}
