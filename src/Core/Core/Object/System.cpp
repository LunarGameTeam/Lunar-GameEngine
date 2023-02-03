#include "Core/Object/System.h"
#include "Core/Object/Component.h"
#include "Core/Object/Entity.h"


namespace luna
{

CORE_API ECSModule* sECSModule = nullptr;
CORE_API LArray<LType*> systems;
void RegisterSystem(LType* system)
{
	systems.push_back(system);  
}

RegisterTypeEmbedd_Imp(System)
{
	cls->Ctor<System>();
	BindingModule::Get("luna")->AddType(cls);
	RegisterSystem(cls);
}

Component* System::AddComponent(Entity* owner, LType* type)
{
	Component* comp = type->NewInstance<Component>();
	comp->SetParent(owner);
	comp->mOwnerEntity = owner;
	comp->mOnCreateCalled = true;
	comp->OnCreate();
	comp->OnActivate();
	TPPtrArray<Component>& owners = mOwnerComponents[type];
	owners.PushBack(comp);
	for (auto it : mRequireComponent)
	{
		TPPtrArray<Component>& requireComp = it.second;
		requireComp.PushBack(owner->GetComponentByType(type));
	}
	return comp;
}


RegisterTypeEmbedd_Imp(ECSModule)
{
	cls->Ctor<ECSModule>();
	BindingModule::Get("luna")->AddType(cls);
}

ECSModule::ECSModule()
{
	sECSModule = this;
	mNeedTick = true;
}

bool ECSModule::OnLoad()
{
	for (LType* type : systems)
	{
		System* system = type->NewInstance<System>();
		mSystems.push_back(system);
	}
	return true;
}

bool ECSModule::OnInit()
{
	return true;
}

bool ECSModule::OnShutdown()
{
	return true;
}

void ECSModule::Tick(float delta_time)
{
	for (System* it : mSystems)
	{
		it->OnTick(delta_time);
	}

}

Component* ECSModule::AddComponent(Entity* owner, LType* type)
{
	System* system = nullptr;
	auto it = mComponentSystems.find(type);
	if (it == mComponentSystems.end())
	{
		system = mSystems[0];
	}
	else
	{
		system = it->second;
	}
	Component* comp = system->AddComponent(owner, type);
	return comp;
}

}