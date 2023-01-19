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

	LMatrix4f& GetLocalToWorldMatrix();
	LMatrix4f& GetWorldToLocalMatrix();

	void UpdateMatrix();

	void SetDirty();;
	void SetPosition(const LVector3f &pos);
	void SetRotation(const LQuaternion &pos);
	void SetScale(const LVector3f &pos);

	LVector3f GetPosition()
	{
		return mPos;
	}
	LVector3f GetScale()
	{
		return mScale;
	}
	LQuaternion  GetRotation()
	{
		return mRotation;
	}

	LVector3f RightDirection();
	LVector3f UpDirection();
	LVector3f ForwardDirection();

private:
	LMatrix4f mLocal2World;
	LMatrix4f mWorld2Local;

	bool mMatrixDirty = true;
	LVector3f mPos = LVector3f::Zero();
	LQuaternion mRotation = LQuaternion::Identity();
	LVector3f mScale = LVector3f(1, 1, 1);
};
}