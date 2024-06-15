#include "Game/Scene.h"
#include "Graphics/RenderModule.h"
#include "Game/Light.h"

#include "Core/Memory/PtrBinding.h"
#include "Core/Serialization/JsonSerializer.h"
#include "Core/Asset/AssetModule.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Game/GameModule.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Scene)
{
	cls->Ctor<Scene>();
	cls->BindingProperty<&Self::m_main_light>("main_light");

	cls->Property<&Self::mEntites>("entities")
		.Serialize();
	cls->VirtualProperty("renderable")
		.Getter<&Scene::GetRenderable>()
		.Setter<&Scene::SetRenderable>()
		.Binding<Scene, bool>();

	cls->BindingMethod<&Scene::CreateEntity>("create_entity");
	cls->BindingMethod<&Scene::FindEntity>("find_entity");
	cls->BindingMethod<&Scene::DestroyEntity>("destroy_entity");
	cls->BindingMethod<&Scene::Destroy>("destroy");
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
	if (mRenderScene)
	{
		sGameModule->RemoveScene(this);
		sRenderModule->RemoveScene(mRenderScene);
		mRenderScene = nullptr;
	}
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
	entity->OnCreate();
	entity->OnActivate();
	mEntites.PushBack(entity);
	return entity;
}

void Scene::DestroyEntity(Entity* entity)
{
	mEntites.Erase(entity);
	entity->Destroy();
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
			Component* compPointer = comp.Get();
			if (compPointer->mNeedTick)
			{
				compPointer->OnTick(deltaTime);
			}
		}
	}
}

void Scene::RenderDataCommandFinishRecord()
{
	mRenderScene->GetRenderDataUpdater()->FinishRecord();
}

void Scene::Destroy()
{
	return;
}

void Scene::OnLoad()
{
	mEntites.Erase(nullptr);

	for (auto& entity : mEntites)
	{
		entity->mScene = this;
		entity->OnCreate();		
	}

	for (auto& entity : mEntites)
	{
		entity->SetParent(this);
		Entity* entityPointer = entity.Get();
		for (auto& comp : entityPointer->mComponents)
		{
			comp->mOwnerEntity = entityPointer;
			comp->SetParent(entityPointer);
			if(comp->mOnCreateCalled == false)
			{
				comp->mOnCreateCalled = true;
				comp->OnCreate();
			}			
		}
	}

	for (auto& entity : mEntites)
	{
		if(entity->mActive)
			entity->OnActivate();
		else
			entity->OnDeactivate();

		for (auto& comp : entity->mComponents)
		{
			if (comp->mActive)
				comp->OnActivate();
			else
				comp->OnDeactivate();
		}
	}
}

}
