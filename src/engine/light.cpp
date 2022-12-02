#include "light.h"
#include "core/object/transform.h"
#include "render/render_module.h"
#include "engine/scene_module.h"
#include "engine/camera.h"

namespace luna
{

RegisterTypeEmbedd_Imp(LightComponent)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
	cls->Ctor<LightComponent>();
	cls->Property<&Self::m_color>("color");
};
RegisterTypeEmbedd_Imp(DirectionLightComponent)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
	cls->Ctor<DirectionLightComponent>();
};

RegisterTypeEmbedd_Imp(PointLightComponent)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
	cls->Ctor<PointLightComponent>();
};


void LightComponent::OnCreate()
{
	Component::OnCreate();
}

void DirectionLightComponent::OnCreate()
{
	LightComponent::OnCreate();
	GetScene()->SetMainDirectionLight(this);
}

const float DirectionLightComponent::GetIntensity()const
{
	return m_indensity;
}

const LMatrix4f &DirectionLightComponent::GetProjectionMatrix(int csm_index)const
{
	return m_proj_matrix[csm_index];
}

const LVector3f DirectionLightComponent::GetDirection()const
{
	return mTransform->ForwardDirection();
}

const luna::LMatrix4f DirectionLightComponent::GetWorldMatrix()const
{
	return mTransform->GetLocalToWorldMatrix();
}

const LMatrix4f DirectionLightComponent::GetViewMatrix(int csm_index)const
{
	return m_view_matrix[csm_index];
}

const LMatrix4f DirectionLightComponent::GetViewMatrix()const
{
	return mTransform->GetWorldToLocalMatrix();
}

const LVector3f DirectionLightComponent::GetColor()const
{
	return m_color;
}

const LQuaternion DirectionLightComponent::GetRotation()const
{
	return mTransform->GetRotation();
}

void DirectionLightComponent::SetProjectionMatrix(const LMatrix4f& val, int csm_index)
{
	m_proj_matrix[csm_index] = val;
}

void DirectionLightComponent::SetViewMatrix(const LMatrix4f& val, int csm_index)
{
	m_view_matrix[csm_index] = val;
}

const LVector3f DirectionLightComponent::GetCSMSplit()const
{
	return m_csm_split;
}

void DirectionLightComponent::SetCSMSplits(const LVector3f& val)
{
	m_csm_split = val;
}

void PointLightComponent::OnCreate()
{
	LightComponent::OnCreate();
}

float PointLightComponent::GetIntensity()
{
	return m_indensity;
}

LVector3f &PointLightComponent::GetPosition()
{
	return mTransform->GetPosition();
}
}