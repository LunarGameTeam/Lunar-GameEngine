#include "AssetImport/ImportScene/ImportDataCommon.h"

namespace luna::asset
{
	void ConvertPositionByTransform(bool hasReflectTransform, const LMatrix4f& convertInvMatrix, const LMatrix4f& convertMatrix, LVector3f& pos)
	{
		LTransform translationMid;
		translationMid.setIdentity();
		translationMid.translate(pos);
		LMatrix4f transDataAns = convertInvMatrix * translationMid.matrix() * convertMatrix;
		pos = transDataAns.block<3, 1>(0, 3);
	}

	void ConvertScaleByTransform(bool hasReflectTransform, const LMatrix4f& convertInvMatrix, const LMatrix4f& convertMatrix, LVector3f& scale)
	{
		LTransform scalMid;
		scalMid.setIdentity();
		scalMid.scale(scale);
		LMatrix4f scalDataAns = convertInvMatrix * scalMid.matrix() * convertMatrix;
		scale.x() = scalDataAns(0, 0);
		scale.y() = scalDataAns(1, 1);
		scale.z() = scalDataAns(2, 2);
	}

	void ConvertRotationByTransform(bool hasReflectTransform, const LMatrix4f& convertInvMatrix, const LMatrix4f& convertMatrix, LQuaternion& rotation)
	{
		LMatrix4f matrixRotation = LMatrix4f::Identity();
		matrixRotation.block(0, 0, 3, 3) = rotation.toRotationMatrix();
		LMatrix4f rotationDataAns = convertInvMatrix * matrixRotation * convertMatrix;
		LMatrix3f rotationDataMat = rotationDataAns.block(0, 0, 3, 3);
		rotation = LQuaternion(rotationDataMat);
	}

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

	void LImportNodeAnimationBase::ConvertAnimationAxisAndUnit(bool hasReflectTransform, LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		ConvertAnimationAxisAndUnitImpl(hasReflectTransform, convertInvMatrix, convertMatrix);
	}
}

