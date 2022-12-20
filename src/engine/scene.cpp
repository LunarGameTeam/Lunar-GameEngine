#include "scene.h"
#include "render/render_module.h"
#include "engine/light.h"
#include "core/serialization/serialization.h"
#include "core/asset/asset_module.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Scene)
{
	cls->Ctor<Scene>();
	cls->BindingProperty<&Self::m_main_light>("main_light");
	cls->Property<&Self::m_entities>("entities");
	cls->BindingMethod<&Scene::FindEntity>("find_entity");
	cls->BindingMethod<&Scene::GetEntityAt>("get_entity_at");
	cls->BindingMethod<&Scene::GetEntityCount>("get_entity_count");
	cls->Binding<Scene>();
	BindingModule::Get("luna")->AddType(cls);
};


Scene::Scene() :
	m_entities(this)
{
	mRenderScene = sRenderModule->AddScene();
}

Entity *Scene::FindEntity(const LString &name)
{
	for (TSubPtr<Entity> &entity : m_entities)
	{
		if (entity->GetObjectName() == name)
			return entity.Get();
	}
	return nullptr;
}

Entity *Scene::CreateEntity(const LString &name, Entity *parent /*= nullptr*/)
{
	auto entity = TCreateObject<Entity>();
	entity->mName = name;
	entity->mScene = this;
	m_entities.PushBack(entity);
	return entity;
}

void Scene::SetMainDirectionLight(DirectionLightComponent *light)
{
	m_main_light = light;
}

const TSubPtrArray<Entity>& Scene::GetAllEntities()
{
	return m_entities;
};

void Scene::Tick(float deltaTime)
{
	for (TSubPtr<Entity>& entity : m_entities)
	{
		for (TSubPtr<Component>& comp : entity->m_components)
		{
			if (comp->mNeedTick)
			{
				comp->OnTick(deltaTime);
			}
		}
	}
}

DirectionLightComponent* Scene::GetMainDirectionLight()
{
	return m_main_light;
}

void Scene::OnLoad()
{
	for (TSubPtr<Entity>& entity : m_entities)
	{
		entity->mScene = this;
		entity->OnCreate();		
	}
	for (TSubPtr<Entity>& entity : m_entities)
	{
		for (TSubPtr<Component>& comp : entity->m_components)
		{
			comp->mOwnerEntity = entity.Get();
			if(comp->mOnCreateCalled == false)
			{
				comp->mOnCreateCalled = true;
				comp->OnCreate();
			}			
		}
	}
}

}