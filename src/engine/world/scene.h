#pragma once

#include "core/core_module.h"
#include "world/entity.h"
#include "core/subsystem/sub_system.h"

namespace luna
{

class Scene : LObject
{
public:
	Entity *CreateEntity(LString& name);

private:
	LVector<Entity *> m_entities;
};

class SceneManager
{

};

class WorldSubsystem : public SubSystem
{
public:
	bool OnPreInit() override;

	bool OnPostInit() override;

	bool OnInit() override;

	bool OnShutdown() override;

	void Tick() override;

};


}
