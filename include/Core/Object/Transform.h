#pragma once


#include "Core/Object/Component.h"
#include "Core/Math/Math.h"
#include "Core/Foundation/Signal.h"

namespace luna
{
class CORE_API Transform : public Component
{
	RegisterTypeEmbedd(Transform, Component);

public:
	SIGNAL(OnTransformDirty, Transform*);
		
public:
	void OnCreate() override;
	void SetTransformDirty();

	void LookAt(const LVector3f& forward);

	Transform* GetParentTransform()
	{
		return mParent;
	}

	LMatrix4f& GetLocalToWorldMatrix();
	LMatrix4f& GetWorldToLocalMatrix();

	void UpdateMatrix();

	void SetDirty();;
	void SetPosition(const LVector3f &pos);
	void SetRotation(const LQuaternion &pos);
	void SetScale(const LVector3f &pos);

	GETTER_REF(LVector3f, mPos, Position);
	GETTER_REF(LQuaternion, mRotation, Rotation);
	GETTER_REF(LVector3f, mScale, Scale);

	LVector3f GetWorldPosition();
	LVector3f GetWorldScale();
	LQuaternion  GetWorldRoatation();

	LVector3f RightDirection();
	LVector3f UpDirection();
	LVector3f ForwardDirection();

private:
	LMatrix4f mLocal2World;
	LMatrix4f mWorld2Local;
	Transform* mParent;

	bool mMatrixDirty = true;
	LVector3f mWorldPos= LVector3f::Zero();
	LVector3f mWorldScale = LVector3f(1, 1, 1);
	LQuaternion mWorldRotation = LQuaternion::Identity();


	LVector3f mPos = LVector3f::Zero();
	LQuaternion mRotation = LQuaternion::Identity();
	LVector3f mScale = LVector3f(1, 1, 1);
};
}