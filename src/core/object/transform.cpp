#include "transform.h"
#include "component.h"
#include "entity.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Transform)
{
	cls->Ctor<Transform>();

	cls->Property<&Self::mPos>("local_position")		
		.Setter<&Self::SetPosition>();

	cls->Property<&Self::mRotation>("local_rotation")
		.Setter<&Self::SetRotation>();

	cls->Property<&Self::mScale>("local_scale")
		.Setter<&Self::SetScale>();

	cls->Binding<Transform>();
	LBindingModule::Luna()->AddType(cls);
}

void Transform::OnCreate()
{
	Component::OnCreate();
	Entity* parent = GetEntity()->GetParentEntity();
	mParent = parent ? parent->GetTransform() : nullptr;
	GetLocalToWorldMatrix();
}

luna::LMatrix4f& Transform::GetLocalToWorldMatrix()
{
	UpdateMatrix();
	return mLocal2World;
}

luna::LMatrix4f& Transform::GetWorldToLocalMatrix()
{
	UpdateMatrix();
	return mWorld2Local;
}

void Transform::UpdateMatrix()
{
	if (m_dirty)
	{
		LTransform trans = LTransform::Identity();
		trans.translate(mPos);
		trans.rotate(mRotation);
		trans.scale(mScale);

		if (mParent)
			mLocal2World = mParent->GetLocalToWorldMatrix() * trans.matrix();
		else
			mLocal2World = trans.matrix();


		LTransform invers = trans.inverse();

		if (mParent)		
			mWorld2Local = mParent->GetWorldToLocalMatrix()* invers.matrix();
		else
			mWorld2Local = invers.matrix();
		 
		m_dirty = false;
	}
}

void Transform::SetDirty()
{
	m_dirty = true;
}

void Transform::SetPosition(const LVector3f &pos)
{
	mPos = pos;
	m_dirty = true;
	UpdateMatrix();
}

void Transform::SetRotation(const LQuaternion &rota)
{
	mRotation = rota;
	m_dirty = true;
	UpdateMatrix();
}

void Transform::SetScale(const LVector3f &scale)
{
	mScale = scale;
	m_dirty = true;
	UpdateMatrix();
}

luna::LVector3f Transform::GetWorldPosition()
{
	if(m_dirty)
		mWorldPos = LMath::xyz(GetLocalToWorldMatrix() * LMath::xyzw(mPos));
	return mWorldPos;
}

luna::LVector3f Transform::GetWorldScale()
{
	if (m_dirty)
		mWorldScale = LMath::xyz(GetLocalToWorldMatrix() * LMath::xyzw(mScale));
	return mWorldScale;
}

luna::LQuaternion Transform::GetWorldRoatation()
{
	if (m_dirty)
	{
		mWorldRotation = mRotation * GetWorldRoatation();
		UpdateMatrix();
	}		
	return mWorldRotation;
}

luna::LVector3f Transform::RightDirection()
{
	LVector3f result = mRotation * LVector3f::UnitX();
	result.normalize();
	return result;
}

luna::LVector3f Transform::UpDirection()
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
	m_dirty = true;
}

void Transform::LookAt(const LVector3f &forward)
{
	mRotation = LQuaternion::FromTwoVectors(LVector3f(0, 0, 1), forward);
}

}