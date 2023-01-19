#pragma once


#include "Core/Object/BaseObject.h"
#include "Component.h"

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

	Scene* GetScene();
	LObject* PyGetScene();

	Transform *GetTransform();

	template<typename T>
	T *GetComponent()
	{
		for (auto& it : mComponents)
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
		for (const auto& it : mComponents)
		{
			T* res = dynamic_cast<T*>(it.Get());
			if (res != nullptr)
				return res;
		}
		return nullptr;
	}

	Component* GetComponentByType(LType* val)
	{

		for (auto& it : mComponents)
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
		return static_cast<T*>(AddComponent(LType::Get<T>()));
	}

	Component* AddComponent(LType* type);

	TPPtrArray<Component> &GetAllComponents()
	{
		return mComponents;
	}
	const TPPtrArray<Component>& GetAllComponents()const
	{
		return mComponents;
	}
	Component* GetComponentAt(int idx)
	{
		if(idx < mComponents.Size())
			return mComponents[idx];
		return nullptr;
	}

	size_t GetComponetCount()
	{
		return mComponents.Size();
	}

	bool GetActive();
	bool GetActiveSelf();
	void SetActive(bool value);

	void UpdateActiveStatus(bool val);


protected:
	Transform *mTransform = nullptr;

private:
	TPPtrArray<Component> mComponents;

	bool mInited = false;
	bool mNeedTick = false;
	bool mOnCreateCalled = false;

	bool mActive = false;

	Scene *mScene;

	friend class Scene;
	friend class Component;
	friend class GameModule;
};
}