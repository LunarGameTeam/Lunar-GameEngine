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

void Component::Destroy()
{
	delete this;
}

Scene *Component::GetScene()
{
	return mOwnerEntity->GetScene();
}

}