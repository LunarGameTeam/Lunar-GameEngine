#include "Game/Light.h"
#include "Core/Object/Transform.h"
#include "Graphics/RenderModule.h"
#include "Game/GameModule.h"
#include "Game/Camera.h"

namespace luna
{

RegisterTypeEmbedd_Imp(LightComponent)
{	
	cls->Ctor<LightComponent>();
	cls->Binding<Self>();

	cls->BindingProperty<&Self::mColor>("color")
		.Serialize();
	BindingModule::Get("luna")->AddType(cls);
};

RegisterTypeEmbedd_Imp(DirectionLightComponent)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
	cls->Ctor<DirectionLightComponent>();
};

RegisterTypeEmbedd_Imp(PointLightComponent)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
	cls->Ctor<PointLightComponent>();
};


void LightComponent::OnCreate()
{
	Component::OnCreate();
	mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&Self::OnTransformDirty, this));
}

void LightComponent::OnTransformDirty(Transform* transform)
{
	GetScene()->GetRenderScene()->SetSceneBufferDirty();
	mLight->mDirection = transform->ForwardDirection();
	
}

void DirectionLightComponent::OnCreate()
{
	LightComponent::OnCreate();
	mLight = GetScene()->GetRenderScene()->CreateMainDirLight();
	OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
}

const float DirectionLightComponent::GetIntensity()const
{
	return mIdentisity;
}

const LMatrix4f &DirectionLightComponent::GetProjectionMatrix(int csm_index)const
{
	return mProj[csm_index];
}

const LVector3f DirectionLightComponent::GetDirection() const
{
	return mTransform->ForwardDirection();
}

const luna::LMatrix4f DirectionLightComponent::GetWorldMatrix()const
{
	return mTransform->GetLocalToWorldMatrix();
}

const LMatrix4f DirectionLightComponent::GetViewMatrix(int idx)const
{
	return mViewMatrix[idx];
}

const LMatrix4f DirectionLightComponent::GetViewMatrix()const
{
	return mTransform->GetWorldToLocalMatrix();
}

const LVector3f DirectionLightComponent::GetColor()const
{
	return mColor;
}

const LQuaternion DirectionLightComponent::GetRotation()const
{
	return mTransform->GetRotation();
}

void DirectionLightComponent::SetProjectionMatrix(const LMatrix4f& val, int csm_index)
{
	mProj[csm_index] = val;
}

void DirectionLightComponent::SetViewMatrix(const LMatrix4f& val, int csm_index)
{
	mViewMatrix[csm_index] = val;
}

const LVector3f DirectionLightComponent::GetCSMSplit()const
{
	return mCSMSplit;
}

void DirectionLightComponent::SetCSMSplits(const LVector3f& val)
{
	mCSMSplit = val;
}

void PointLightComponent::OnCreate()
{
	LightComponent::OnCreate();
}

float PointLightComponent::GetIntensity()
{
	return mIdentisity;
}

LVector3f PointLightComponent::GetPosition()
{
	return mTransform->GetPosition();
}
}