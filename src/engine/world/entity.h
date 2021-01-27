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

	}

private:
	LVector<Component *> m_components;
};

}