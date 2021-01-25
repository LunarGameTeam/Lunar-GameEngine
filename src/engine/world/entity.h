#pragma once

#include "core/core_module.h"
#include "component.h"

namespace luna
{

class Entity : public LObject
{
public:
	Entity(const LString &name)
	{
		
	}
	Entity(const Entity &rv) = delete;

	void OnCreate();
	void OnDestroy();

	template<typename T>
	T *AddComponent()
	{
		T *comp = new T();		
		m_components.push_back(comp);
		comp->OnCreate();
		return T;
	}

private:
	LVector<Component *> m_components;
};

}