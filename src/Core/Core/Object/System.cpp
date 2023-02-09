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
	Component* comp = type->NewInstance<Component>();
	comp->SetParent(owner);
	comp->mOwnerEntity = owner;
	comp->mOnCreateCalled = true;
	comp->OnCreate();
	comp->OnActivate();
	mComponents[type].PushBack(comp);
	return comp;
}

}