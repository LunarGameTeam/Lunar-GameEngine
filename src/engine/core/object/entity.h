#pragma once

#include "core/private_core.h"
#include "core/object/object.h"
#include "component.h"


namespace luna
{

class Transform;

class CORE_API Entity : public LObject
{
public:
	Entity(const Entity &rv) = delete;

	void OnCreate();
	void OnDestroy();
	void OnTick(float delta_time);
	void Destroy();
	Entity *Parent();

	Transform *GetTransform();

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
	T *RequireComponent()
	{
		T *t = GetComponent<T>();
		if (t == nullptr)
		{
			t = AddComponent<>();
		}
		return t
	}

	template<typename T>
	T *AddComponent()
	{
		T *comp = new T();
		comp->m_owner = this;
		m_components.push_back(comp);
		comp->OnCreate();
		return comp;
	}
protected:
	Entity(const LString &name);
	Transform *m_cached_transform;

private:
	LVector<Component *> m_components;
	Transform *m_transform = nullptr;
	Transform *m_parent = nullptr;
	bool m_need_tick = false;

	friend class Scene;
	friend class WorldSubsystem;
};

}