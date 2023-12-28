#pragma once

#include "Core/Object/BaseObject.h"


namespace luna
{
class Entity;
class Transform;
class Scene;
class System;

class CORE_API Component : public LObject
{
	RegisterTypeEmbedd(Component, LObject)
public:
	static LType* GetSystemType();

	Component(const Component &comp) = delete;

	Entity *GetEntity()
	{
		return mOwnerEntity;
	}

	void   Destroy();
	Scene* GetScene();

	void ForceCreate();

	virtual void OnDeactivate();
	virtual void OnActivate();
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnTick(float delta_time);
	bool CheckIsRenderComponent();
	void UpdateActiveState(bool val);
protected:
	Component();
	bool mNeedTick = false;
	Transform *mTransform = nullptr;
	Entity *mOwnerEntity = nullptr;
	bool mIsRenderComponent = false;
private:
	bool mOnCreateCalled = false;	
	bool mActive = true;


	friend class Entity;
	friend class Scene;
	friend class GameModule;

	friend class System;
	friend class ECSModule;
};
}