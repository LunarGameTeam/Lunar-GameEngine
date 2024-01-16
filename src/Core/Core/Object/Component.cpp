#include "Core/Object/Component.h"
#include "Core/Object/Entity.h"
#include "Core/Object/System.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Component)
{
	cls->Ctor<Component>();
	cls->Binding<Component>();
	cls->BindingProperty<&Component::mOwnerEntity>("entity");
	cls->BindingMethod<&Component::Destroy>("destroy");	
	cls->BindingMethod<&Component::ForceCreate>("force_create");
	BindingModule::Get("luna")->AddType(cls);
}

LType* Component::GetSystemType()
{
	return LType::Get<System>();
}

Component::Component()
{
}

void Component::OnActivate()
{

}


void Component::OnDeactivate()
{
}

void Component::OnCreate()
{
	mTransform = GetEntity()->GetTransform();
	mOnCreateCalled = true;
}

void Component::ForceCreate()
{
	OnCreate();
	OnActivate();
}

void Component::OnDestroy()
{
}

void Component::OnTick(float delta_time)
{
}

void Component::UpdateActiveState(bool val)
{
	bool new_active = val;
	bool old_active = mActive;
	if (mOwnerEntity)
	{
		new_active = mActive && mOwnerEntity->GetActive();
	}
	mActive = new_active;
	if (old_active)
	{
		if (!mActive)
			OnDeactivate();
	}
	else
		if (mActive)
				OnActivate();
}

bool Component::CheckIsRenderComponent()
{
	return mIsRenderComponent;
}

void Component::Destroy()
{
	delete this;
}

Scene *Component::GetScene()
{
	if(mParent)
		return (Scene*)(mParent->GetParent());
	return nullptr;
}

}