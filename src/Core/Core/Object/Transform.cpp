#include "Core/Object/Transform.h"
#include "Core/Object/Component.h"
#include "Core/Object/Entity.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Transform)
{
	cls->Ctor<Transform>();

	cls->BindingProperty<&Self::mPos>("local_position")
		.Setter<&Self::SetPosition>()
		.Serialize();

	cls->BindingProperty<&Self::mRotation>("local_rotation")
		.Setter<&Self::SetRotation>()
		.Serialize();

	cls->BindingProperty<&Self::mScale>("local_scale")
		.Setter<&Self::SetScale>()
		.Serialize();

	cls->BindingProperty<&Self::mLocal2World>("world_matrix")
		.Getter<&Transform::GetLocalToWorldMatrix>();

	cls->Binding<Transform>();
	BindingModule::Luna()->AddType(cls);
}

void Transform::OnCreate()
{
	Component::OnCreate();
	GetLocalToWorldMatrix();
}

LMatrix4f& Transform::GetLocalToWorldMatrix()
{
	UpdateMatrix();
	return mLocal2World;
}

LMatrix4f& Transform::GetWorldToLocalMatrix()
{
	UpdateMatrix();
	return mWorld2Local;
}

void Transform::UpdateMatrix()
{
	if (mMatrixDirty)
	{
		LTransform trans = LTransform::Identity();

		trans.translate(mPos);

		trans.rotate(mRotation);

		trans.scale(mScale);

		mLocal2World = trans.matrix();		
		mWorld2Local = mLocal2World.inverse();		
		mMatrixDirty = false;
	}
}

void Transform::SetDirty()
{
	mMatrixDirty = true;
}

void Transform::SetPosition(const LVector3f &pos)
{
	mPos = pos;
	mMatrixDirty = true;
	UpdateMatrix();
	OnTransformDirty.BroadCast(this);
}

void Transform::SetRotation(const LQuaternion &rota)
{
	mRotation = rota;
	mMatrixDirty = true;
	UpdateMatrix();
	OnTransformDirty.BroadCast(this);
}

void Transform::SetScale(const LVector3f &scale)
{
	mScale = scale;
	mMatrixDirty = true;
	UpdateMatrix();
	OnTransformDirty.BroadCast(this);
}


LVector3f Transform::RightDirection()
{
	LVector3f result = mRotation * LVector3f::UnitX();
	result.normalize();
	return result;
}

LVector3f Transform::UpDirection()
{
	LVector3f result = mRotation * LVector3f::UnitY();
	result.normalize();
	return result;
}

luna::LVector3f Transform::ForwardDirection()
{
	LVector3f result = mRotation * LVector3f::UnitZ();
	result.normalize();
	return result;
}

void Transform::SetTransformDirty()
{
	mMatrixDirty = true;
}

void Transform::LookAt(const LVector3f &forward)
{
	mRotation = LQuaternion::FromTwoVectors(LVector3f(0, 0, 1), forward);
}

}