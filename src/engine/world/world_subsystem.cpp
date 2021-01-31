#include "world_subsystem.h"
#include "scene.h"
#include "entity.h"

namespace luna
{

bool WorldSubsystem::OnPreInit()
{
	return true;
}

bool WorldSubsystem::OnPostInit()
{
	return true;
}

bool WorldSubsystem::OnInit()
{
	m_need_tick = true;
	return true;
}

bool WorldSubsystem::OnShutdown()
{
	return true;
}

void WorldSubsystem::Tick(float delta_time)
{
	SceneManager *manager = SceneManager::instance();
	Scene *main_scene = manager->MainScene();
	for (auto& entity : main_scene->m_entities)
	{
		if (entity->m_need_tick)
		{
			entity->OnTick(delta_time);
		}
		for (auto &comp : entity->m_components)
		{
			if (comp->m_need_tick)
			{
				comp->OnTick(delta_time);
			}
		}
	}
}

}