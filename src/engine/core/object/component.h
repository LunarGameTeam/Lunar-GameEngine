#pragma once

#include "core/object/object.h"
#include "core/private_core.h"

namespace luna
{
class Entity;

class CORE_API Component : public LObject
{
public:
	Component(const Component &comp) = delete;

	inline Entity *GetEntity()
	{
		return m_owner;
	}
	void Destroy();

	virtual void OnDisable();
	virtual void OnEnable();
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual void OnTick(float delta_time);
protected:
	Component();
	bool m_need_tick = false;
private:
	bool m_is_initliazed = false;
	bool m_enable = true;
	Entity *m_owner;

	friend class Entity;
	friend class WorldSubsystem;
};

}