#pragma once

#include "engine_config.h"
#include "core/core_library.h"
#include "core/object/entity.h"
#include "core/framework/module.h"
#include "core/asset/json_asset.h"

#include "render/renderer/render_scene.h"

namespace luna
{

class DirectionLightComponent;

class ENGINE_API Scene : public LJsonAsset
{
	RegisterTypeEmbedd(Scene, LJsonAsset)
public:
	Scene();

	Entity *FindEntity(const LString &name);
	Entity *CreateEntity(const LString &name, Entity *parent = nullptr);

	void SetMainDirectionLight(DirectionLightComponent *light);
	DirectionLightComponent* GetMainDirectionLight();

	const TSubPtrArray<Entity>& GetAllEntities();

	void Tick(float deltaTime);

	render::RenderScene* GetRenderScene() { return mRenderScene; };
public:
	
	void OnLoad() override;

private:
	bool mInit = false;
	render::RenderScene* mRenderScene;
	TSubPtr<DirectionLightComponent> m_main_light;
	TSubPtrArray<Entity> m_entities;

	friend class SceneModule;
};
}
