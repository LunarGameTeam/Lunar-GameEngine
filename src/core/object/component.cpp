#include "component.h"
#include "core/object/entity.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Component)
{
	cls->Ctor<Component>();
	cls->Binding<Component>();
	BindingModule::Get("luna")->AddType(cls);
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

void Component::UpdateActiveState()
{
	bool new_active = mActiveSelf;
	bool old_active = mActive;
	if (mOwnerEntity)
	{
		new_active = mActiveSelf && mOwnerEntity->GetActive();
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
}

Scene *Component::GetScene()
{
	return mOwnerEntity->GetScene();
}

}