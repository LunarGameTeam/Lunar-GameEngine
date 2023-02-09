#pragma once

#include "Core/Object/BaseObject.h"
#include "Core/Framework/Module.h"
#include "Core/CoreTypes.h"

#include <functional>

namespace luna
{

CORE_API extern ECSModule* sECSModule;
CORE_API extern LArray<LType*> systems;
CORE_API void RegisterSystem(LType* system);

class CORE_API ECSModule : public LModule
{
	RegisterTypeEmbedd(ECSModule, LModule);
public:
	ECSModule();
	virtual ~ECSModule() {};
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;


	Component* AddComponent(Entity* owner, LType* type);

	LMap<LType*, TPPtrArray<Component>> mComponents;
	LMap<LType*, LArray<System*>>       mComponentSystems;
	LArray<System*>                     mSystems;

};

class CORE_API System : public LObject
{
	RegisterTypeEmbedd(System, LObject)
public:

	System()
	{

	}

	virtual void OnCreate(Component* comp)
	{

	}

	virtual void OnTick(float delta_time)
	{

	}

	template<typename T>
	void Foreach(std::function<void(uint32_t idx, T* comp, Entity* entity)> func)
	{
		LType* type = LType::Get<T>();
		auto components = sECSModule->mComponents[type];
		uint32_t idx = 0;
		for (auto& it : components)
		{
			func(idx, (T*)it.Get(), it->GetEntity());
			idx++;
		}
	}

	template<typename T>
	void FocusComponentType()
	{
		LType* type = LType::Get<T>();		
		sECSModule->mComponentSystems[type].push_back(this);
	}

};



}