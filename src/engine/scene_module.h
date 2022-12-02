#pragma once

#include "core/core_library.h"
#include "core/framework/module.h"
#include "scene.h"
#include "pch.h"

namespace luna
{
class CameraComponent;

class ENGINE_API SceneModule : public LModule
{
	RegisterTypeEmbedd(SceneModule, LModule)
public:
	SceneModule();;
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

	void BindingAddScene(Scene* new_scene);

	void AddScene(LType* new_scene);

	Scene* GetScene(int32_t index_scene) { return mScenes[index_scene]; }

	void OnIMGUI() override;
private:
	TSubPtrArray<Scene> mScenes;
};
}
