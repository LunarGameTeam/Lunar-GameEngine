#define RESOURCE_IMPORT_EXPORT
#include"import_data_common.h"
namespace luna::resimport
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

	void LImportNodeDataBase::ConvertDataAxisAndUnit(LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		ConvertDataAxisAndUnitImpl(convertInvMatrix, convertMatrix);
	}
}

