#include "AssetImport/ImportScene/ImportDataCommon.h"

namespace luna::asset
{
	LImportSceneNode::LImportSceneNode() :mIndex(-1), mParent(-1), mName("")
	{
		mTranslation.setZero();
		mRotation.setIdentity();
		mScal.setZero();
		mExtrMatrix.setIdentity();
	}

	LImportNodeDataBase::LImportNodeDataBase(const LImportNodeDataType type, const size_t index) :mType(type), mIndex(index)
	{
	};

	void LImportNodeDataBase::ConvertDataAxisAndUnit(bool hasReflectTransform,LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		ConvertDataAxisAndUnitImpl(hasReflectTransform, convertInvMatrix, convertMatrix);
	}


	LImportNodeAnimationBase::LImportNodeAnimationBase(const LImportNodeAnimationType type, const size_t index) :mType(type), mIndex(index)
	{

	}

	void LImportNodeAnimationBase::ConvertDataAxisAndUnit(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		ConvertDataAxisAndUnitImpl(hasReflectTransform, convertInvMatrix, convertMatrix);
	}
}

