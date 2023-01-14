#pragma once

#include "core/object/base_object.h"


namespace luna
{
class Entity;
class Transform;
class Scene;

class CORE_API Component : public LObject
{
	RegisterTypeEmbedd(Component, LObject)
public:
	Component(const Component &comp) = delete;

	inline Entity *GetEntity()
	{
		return mOwnerEntity;
	}
	void Destroy();
	Scene *GetScene();
	virtual void OnDeactivate();
	virtual void OnActivate();
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnTick(float delta_time);

	void UpdateActiveState();
protected:
	Component();
	bool mNeedTick = false;
	Transform *mTransform = nullptr;
	Entity *mOwnerEntity;

private:
	bool mOnCreateCalled = false;
	bool mActiveSelf = true;
	bool mActive = false;


	friend class Entity;
	friend class Scene;
	friend class GameModule;

};
}