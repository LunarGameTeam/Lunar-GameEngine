#pragma once
#include "AssetImport/ImportScene/ImportDataSkeletonAnimation.h"
namespace luna::asset
{
	LImportNodeDataSkeletonAnimation::LImportNodeDataSkeletonAnimation(const size_t index) :LImportNodeAnimationBase(LImportNodeAnimationType::ImportSkeletonAnimation, index)
	{

	}

	void LImportNodeDataSkeletonAnimation::ConvertDataAxisAndUnitImpl(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
	}
}

