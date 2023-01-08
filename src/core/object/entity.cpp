#include "core/object/entity.h"
#include "core/object/transform.h"
#include "core/memory/ptr_binding.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Entity)
{
	cls->Ctor<Entity>();	

	cls->BindingProperty<&Self::mName>("name")
		.Serialize();

	cls->Property<&Self::m_components>("component_list")
		.Serialize();

	cls->Property<&Self::m_children>("children");


	cls->BindingMethod<&Entity::PyGetScene>("get_scene");

	cls->BindingMethod<&Entity::GetComponentByType>("get_component").GetBindingMethodDef().ml_doc = LString::MakeStatic("def get_component(self, t: typing.Type[T] ) -> T:");

	cls->BindingMethod<&Entity::GetComponentAt>("get_component_at");

	cls->BindingMethod<&Entity::Destroy>("destroy");

	cls->BindingMethod<&Entity::GetComponetCount>("get_component_count");

	cls->Binding<Entity>();
	BindingModule::Get("luna")->AddType(cls);
}

void Entity::OnCreate()
{
	mChildren = RequireComponent<Transform>();
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

Entity *Entity::Parent()
{
	if (!mParent) return nullptr;
	return mParent;
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
	return mChildren;
}

bool Entity::GetActive()
{
	return mActive;
}

bool Entity::GetActiveSelf()
{
	return mActiveSelf;
}

void Entity::SetActiveSelf(bool value)
{
	mActiveSelf = value;
	UpdateActiveStatus();
}

void Entity::UpdateActiveStatus()
{
	bool new_active = mActiveSelf;
	bool old_active = mActive;
	if (mParent)
	{
		auto parent = mParent;
		new_active = mActiveSelf && parent->GetActive();	
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

	for (auto& comp : m_components)
	{
		comp->UpdateActiveState();
	}
	for (auto entity : m_children)
	{
		entity->UpdateActiveStatus();
	}
}

bool Entity::AddChild(Entity* child)
{
	auto parent = child->mParent;
	if (parent == this)
		return true;
	if (parent)
	{
		for (auto& it : m_children)
		{
			if (it.Get() == child)
			{
				parent->m_children.Erase(it.Get());
				break;
			}
		}
	}
	m_children.PushBack(child);
	child->mParent = this;
	return true;
}

Entity::Entity():
	m_components(this),
	m_children(this)
{
	mNeedTick = false;
}

}