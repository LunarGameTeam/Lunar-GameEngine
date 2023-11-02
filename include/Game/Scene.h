#pragma once

#include "Game/GameConfig.h"
#include "Core/Object/Entity.h"
#include "Core/Framework/Module.h"
#include "Core/Asset/JsonAsset.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/Renderer/RenderScene.h"


namespace luna
{

class DirectionLightComponent;

class GAME_API Scene : public JsonAsset
{
	RegisterTypeEmbedd(Scene, JsonAsset)
public:
	Scene();
	~Scene();

	Entity *FindEntity(const LString &name);
	Entity *CreateEntity(const LString &name, Entity *parent = nullptr);
	void DestroyEntity(Entity* entity);

	const TPPtrArray<Entity>& GetAllEntities();

	uint32_t GetEntityCount()
	{
		return (uint32_t)mEntites.Size();
	}

	void SetRenderable(bool val)
	{
		mRenderScene->mRenderable = val;
	}
	bool GetRenderable() const { return mRenderScene->mRenderable; }

	Entity* GetEntityAt(uint32_t idx) 
	{
		if (idx < mEntites.Size())
			return mEntites[idx];
		return nullptr;
	}

	void Tick(float deltaTime);

	graphics::RenderScene* GetRenderScene() { return mRenderScene; };
public:
	void Destroy();
	void OnLoad() override;

private:
	bool                     mInit = false;
	graphics::RenderScene*     mRenderScene;
	DirectionLightComponent* m_main_light;
	TPPtrArray<Entity>       mEntites;

	friend class GameModule;
};

}
