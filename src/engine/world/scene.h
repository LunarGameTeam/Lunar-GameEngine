#pragma once

#include "core/core_module.h"
#include "world/entity.h"
#include "core/subsystem/sub_system.h"

namespace luna
{

class Scene : LObject
{

public:
	Entity *CreateEntity(LString &name)
	{
		auto entity = new Entity(name);
		m_entities.insert(entity);
	}
	
	void OnCreate();
	void OnDestroy();

protected:
	Scene()
	{

	}

private:
	LUnorderedSet<Entity*> m_entities;

	friend class SceneManager;
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
