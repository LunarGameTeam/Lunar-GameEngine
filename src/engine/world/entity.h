#pragma once

#include "core/core_module.h"
#include "component.h"

namespace luna
{

class Entity : public LObject
{
public:
	Entity(const Entity &rv) = delete;

	void OnCreate();
	void OnDestroy();
	void OnTick(float delta_time);
	void Destroy();

	template<typename T>
	T *GetComponent()
	{
		for (auto &it : m_components)
		{
			T *res = dynamic_cast<T *>(it);
			if (res != nullptr)
				return res;
			
		}
		return nullptr;
	}

	template<typename T>
	T *AddComponent()
	{
		T *comp = new T();		
		comp->Owner = this;
		m_components.push_back(comp);		
		comp->OnCreate();
		return T;
	}
protected:
	Entity(const LString &name)
	{
		m_need_tick = false;
	}

private:
	LVector<Component *> m_components;
	bool m_need_tick = false;

	friend class Scene;
	friend class WorldSubsystem;
};

}