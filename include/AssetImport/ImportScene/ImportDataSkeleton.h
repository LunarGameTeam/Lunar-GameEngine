#pragma once

#include "AssetImport/ImportScene/ImportDataCommon.h"

namespace luna::asset
{
	struct SkeletonBoneData
	{
		LString mBoneName;
		int32_t mLocalId;
		int32_t mParentId;
		LVector3f mInitLocation;
		LQuaternion mInitRotation;
		LVector3f mInitScal;
	};
	class ASSET_IMPORT_API LImportNodeDataSkeleton : public LImportNodeDataBase
	{
		LArray<SkeletonBoneData> skeletonBones;
	public:
		LImportNodeDataSkeleton(const size_t index);

		void AddBoneToData(const SkeletonBoneData& boneData);

		void SortBoneData();

		const LArray<SkeletonBoneData>& GetBones() const { return skeletonBones; };
	private:
		void ConvertDataAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) override;
	};
}

