#include "Core/Object/Entity.h"
#include "Core/Object/Transform.h"
#include "Core/Memory/PtrBinding.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Entity)
{
	cls->Ctor<Entity>();	

	cls->BindingProperty<&Entity::mName>("name")
		.Serialize();
	cls->BindingProperty<&Entity::mActive>("active")
		.Setter<&Entity::SetActive>()
		.Serialize();

	cls->Property<&Entity::mComponents>("component_list")
		.Serialize();

	cls->VirtualProperty("component_count")
		.Getter<&Entity::GetComponetCount>()
		.Binding<Entity, size_t>();

	cls->VirtualProperty("owner_scene")
		.Getter<&Entity::PyGetScene>()
		.Binding<Entity, LObject*>();

	cls->BindingMethod<&Entity::GetComponentByType>("get_component")
		.SetDoc("def get_component(self, t: typing.Type[T] ) -> T:");

	cls->BindingMethod<(Component* (Entity::*)(LType*))& Entity::AddComponent>("add_component")
		.SetDoc("def add_component(self, t: typing.Type[T] ) -> T:");

	cls->BindingMethod<&Entity::GetComponentAt>("get_component_at");

	cls->BindingMethod<&Entity::Destroy>("destroy");


	cls->Binding<Entity>();
	BindingModule::Get("luna")->AddType(cls);
}

void Entity::OnCreate()
{
	mOnCreateCalled = true;
	mTransform = RequireComponent<Transform>();
}

void Entity::OnDestroy()
{
}

void Entity::OnActivate()
{

}

void Entity::OnDeactivate()
{

}

void Entity::OnTick(float delta_time)
{
}

void Entity::Destroy()
{
	delete this;
}

Scene *Entity::GetScene()
{
	return mScene;
}

LObject* Entity::PyGetScene()
{
	return (LObject*)mScene;
}

Transform *Entity::GetTransform()
{
	if (!mTransform)
		mTransform = GetComponent<Transform>();
	return mTransform;
}

Component* Entity::AddComponent(LType* type)
{
	Component* comp = type->NewInstance<Component>();
	comp->SetParent(this);
	comp->mOwnerEntity = this;
	mComponents.PushBack(comp);
	comp->mOnCreateCalled = true;
	comp->OnCreate();
	comp->OnActivate();
	return comp;
}

bool Entity::GetActive()
{
	return mActive;
}

void Entity::SetActive(bool value)
{
	mActive = value;
	if (!mOnCreateCalled)
		return;
	UpdateActiveStatus(value);
}

void Entity::UpdateActiveStatus(bool val)
{
	bool newState = val;
	bool oldState = mActive;
	if (oldState)
	{
		if (!mActive)
			OnDeactivate();
	}
	else
	{
		if (mActive)
			OnActivate();
	}

	for (auto& comp : mComponents)
	{
		comp->UpdateActiveState(val);
	}
}

Entity::Entity():
	mComponents(this)
{
	mNeedTick = false;
}

}