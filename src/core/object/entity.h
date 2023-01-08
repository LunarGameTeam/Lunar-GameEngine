#pragma once


#include "core/object/base_object.h"
#include "component.h"

namespace luna
{
class Transform;
class Scene;
class Entity;


class CORE_API Entity : public LObject
{
	RegisterTypeEmbedd(Entity, LObject)
public:
	Entity();
	Entity(const Entity &rv) = delete;

	void OnCreate();
	void OnDestroy();
	void OnActivate();
	void OnDeactivate();
	void OnTick(float delta_time);


	void Destroy();
	Entity *Parent();

	Scene* GetScene();
	LObject* PyGetScene();

	Transform *GetTransform();

	template<typename T>
	T *GetComponent()
	{
		for (auto& it : m_components)
		{
			T *res = dynamic_cast<T *>(it.Get());
			if (res != nullptr)
				return res;
		}
		return nullptr;
	}

	template<typename T>
	const T* GetComponent() const
	{
		for (const auto& it : m_components)
		{
			T* res = dynamic_cast<T*>(it.Get());
			if (res != nullptr)
				return res;
		}
		return nullptr;
	}

	Component* GetComponentByType(LType* val)
	{

		for (auto& it : m_components)
		{
			LType* cls = it->GetClass();
			if(cls->IsDerivedFrom(val))
				return it.Get();
		}
		return nullptr;
	}

	template<typename T>
	T *RequireComponent()
	{
		T *t = GetComponent<T>();
		if (t == nullptr)
		{
			t = AddComponent<T>();
		}
		return t;
	}

	template<typename T>
	T *AddComponent()
	{
		T *comp = TCreateObject<T>();
		comp->SetParent(this);
		comp->mOwnerEntity = this;
		m_components.PushBack(comp);
		comp->mOnCreateCalled = true;
		comp->OnCreate();
		return comp;
	}

	TPPtrArray<Component> &GetAllComponents()
	{
		return m_components;
	}
	const TPPtrArray<Component>& GetAllComponents()const
	{
		return m_components;
	}
	Component* GetComponentAt(int idx)
	{
		if(idx < m_components.Size())
			return m_components[idx];
		return nullptr;
	}

	size_t GetComponetCount()
	{
		return m_components.Size();
	}

	bool GetActive();
	bool GetActiveSelf();
	void SetActiveSelf(bool value);

	void UpdateActiveStatus();

public:
	bool AddChild(Entity* child, size_t index);
	bool AddChild(Entity* child);
	Entity* GetParentEntity() { return mParent; }
	Entity* GetChild(size_t index)
	{
		if (index > m_children.Size())
			return nullptr;
		return m_children[index];
	}
	size_t GetChildCount() const { return m_children.Size(); };

protected:
	Transform *mChildren = nullptr;

private:
	TPPtrArray<Entity> m_children;
	TPPtrArray<Component> m_components;

	Entity *mParent = nullptr;

	bool mInited = false;
	bool mNeedTick = false;

	bool mActive = false;
	bool mActiveSelf = true;

	Scene *mScene;

	friend class Scene;
	friend class Component;
	friend class SceneModule;
};
}