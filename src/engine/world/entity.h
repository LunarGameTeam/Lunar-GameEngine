#pragma once

#include "core/core_module.h"
#include "component.h"


namespace luna
{

class Transform;

class Entity : public LObject
{
public:
	Entity(const Entity &rv) = delete;

	void OnCreate();
	void OnDestroy();
	void OnTick(float delta_time);
	void Destroy();
	Entity *Parent();

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
	Entity(const LString &name);

private:
	LVector<Component *> m_components;
	Transform *m_transform = nullptr;
	Transform *m_parent = nullptr;
	bool m_need_tick = false;

	friend class Scene;
	friend class WorldSubsystem;
};

}