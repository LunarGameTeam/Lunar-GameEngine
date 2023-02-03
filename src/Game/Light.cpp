#include "Game/Light.h"
#include "Core/Object/Transform.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Game/GameModule.h"
#include "Game/Camera.h" 

namespace luna
{

RegisterTypeEmbedd_Imp(LightComponent)
{	
	cls->Ctor<LightComponent>();
	cls->Binding<Self>();

	cls->BindingProperty<&LightComponent::mColor>("color")
		.Setter<&LightComponent::SetColor>()
		.Serialize();
	cls->BindingProperty<&Self::mIntensity>("intensity")
		.Setter<&LightComponent::SetIndensity>()
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
	
}

void LightComponent::SetColor(const LVector3f& val)
{
	if (mLight)
	{
		mLight->mDirty = true;
		mLight->mColor = LVector4f(val.x(), val.y(), val.z(), 1);
	}
	mColor = val;
	
}

void LightComponent::SetIndensity(float val)
{
	if (mLight)
	{
		mLight->mDirty = true;
		mLight->mIntensity = val;
	}
	mIntensity = val;
}

void DirectionLightComponent::OnTransformDirty(Transform* transform)
{
	mLight->mDirty = true;
	render::DirectionLight* light = (render::DirectionLight*)mLight;
	light->mDirection = GetDirection();

}
void DirectionLightComponent::OnCreate()
{
	LightComponent::OnCreate();
	mLight = GetScene()->GetRenderScene()->CreateMainDirLight();
	mLight->mIntensity = mIntensity;
	mLight->mColor = LVector4f(mColor.x(), mColor.y(), mColor.z(), 1);
	mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&DirectionLightComponent::OnTransformDirty, this));
	OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
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
	mCastShadow = false;
	mLight = GetScene()->GetRenderScene()->CreatePointLight();
	mLight->mIntensity = mIntensity;
	mLight->mColor = LVector4f(mColor.x(), mColor.y(), mColor.z(), 1);
	mTransformDirtyAction = mOwnerEntity->GetComponent<Transform>()->OnTransformDirty.Bind(AutoBind(&PointLightComponent::OnTransformDirty, this));
	OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
}

void PointLightComponent::OnTransformDirty(Transform* transform)
{
	GetScene()->GetRenderScene()->SetSceneBufferDirty();
	mLight->mDirty = true;
	render::PointLight* light = (render::PointLight*)mLight;
	light->mPosition = mTransform->GetPosition();

}

}