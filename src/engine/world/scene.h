#pragma once

#include "private_world.h"
#include "core/core_module.h"
#include "core/object/entity.h"
#include "core/subsystem/sub_system.h"

namespace luna
{

class WORLD_API Scene : LObject
{

public:
	Entity *CreateEntity(const LString &name, Entity* parent = nullptr)
	{
		auto entity = new Entity(name);
		m_entities.push_back(entity);
		return entity;
	}
	
	void OnCreate();
	void OnDestroy();

protected:
	Scene()
	{

	}

private:
	LVector<Entity*> m_entities;

	friend class SceneManager;
	friend class WorldSubsystem;
};

class SceneManager
{
public:
	Scene *CreateScene()
	{
		return new Scene();
	}

	Scene *MainScene()
	{
		static Scene *main_scene = nullptr;
		if (main_scene == nullptr)
			main_scene = CreateScene();
		return main_scene;
	}
	static SceneManager *instance()
	{
		static SceneManager manager;
		return &manager;
	}
private:


};


}
