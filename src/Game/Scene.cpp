#include "game/Scene.h"
#include "Graphics/RenderModule.h"
#include "game/Light.h"

#include "Core/Memory/PtrBinding.h"
#include "Core/Serialization/JsonSerializer.h"
#include "Core/Asset/AssetModule.h"

#include "Graphics/Renderer/RenderScene.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Scene)
{
	cls->Ctor<Scene>();
	cls->BindingProperty<&Self::m_main_light>("main_light");

	cls->Property<&Self::mEntites>("entities")
		.Serialize();

	cls->BindingMethod<&Scene::CreateEntity>("create_entity");
	cls->BindingMethod<&Scene::FindEntity>("find_entity");
	cls->BindingMethod<&Scene::DestroyEntity>("destroy_entity");
	cls->BindingMethod<&Scene::GetEntityAt>("get_entity_at");
	cls->BindingMethod<&Scene::GetEntityCount>("get_entity_count");
	cls->Binding<Scene>();
	BindingModule::Get("luna")->AddType(cls);
};


Scene::Scene() :
	mEntites(this)
{
	mRenderScene = sRenderModule->AddScene();
}

Scene::~Scene()
{
	if(mRenderScene)
		sRenderModule->RemoveScene(mRenderScene);
}

Entity *Scene::FindEntity(const LString &name)
{
	for (auto& entity : mEntites)
	{
		if (entity->GetObjectName() == name)
			return entity.Get();
	}
	return nullptr;
}

Entity *Scene::CreateEntity(const LString &name, Entity *parent /*= nullptr*/)
{
	Entity* entity = NewObject<Entity>();
	entity->SetParent(this);
	entity->mName = name;
	entity->mScene = this;
		
	mEntites.PushBack(entity);
	return entity;
}

void Scene::DestroyEntity(Entity* entity)
{
	mEntites.Erase(entity);
	entity->Destroy();
}

void Scene::SetMainDirectionLight(DirectionLightComponent *light)
{
	m_main_light = light;
}

const TPPtrArray<Entity>& Scene::GetAllEntities()
{
	return mEntites;
};

void Scene::Tick(float deltaTime)
{
	for (auto& entity : mEntites)
	{
		for (auto& comp : entity->mComponents)
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
	for (auto& entity : mEntites)
	{
		entity->mScene = this;
		entity->OnCreate();		
	}
	for (auto& entity : mEntites)
	{
		entity->SetParent(this);
		for (auto& comp : entity->mComponents)
		{
			comp->mOwnerEntity = entity.Get();
			comp->SetParent(entity.Get());
			if(comp->mOnCreateCalled == false)
			{
				comp->mOnCreateCalled = true;
				comp->OnCreate();
			}			
		}
	}
}

}