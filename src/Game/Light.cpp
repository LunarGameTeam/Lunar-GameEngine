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
	mNeedTick = true;
	mLight = GetScene()->GetRenderScene()->CreateMainDirLight();
	OnTransformDirty(mOwnerEntity->GetComponent<Transform>());
}

void DirectionLightComponent::OnTick(float delta_time)
{
	mLight->mViewMatrix = mTransform->GetWorldToLocalMatrix();
	mLight->mProjMatrix = mProj[0];	
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
}

}