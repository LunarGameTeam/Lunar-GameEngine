#pragma once

#include "core/framework/module.h"
#include "scene.h"
#include "game_config.h"

namespace luna
{
class CameraComponent;

class GAME_API GameModule : public LModule
{
	RegisterTypeEmbedd(GameModule, LModule)
public:
	GameModule();;
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

	void BindingAddScene(Scene* new_scene);

	void AddScene(LType* new_scene);

	Scene* GetScene(int32_t index_scene) 
	{ 
		if(index_scene < mScenes.Size())
			return mScenes[index_scene]; 
		return nullptr;		   
	}

	void OnIMGUI() override;
private:
	TPPtrArray<Scene> mScenes;
};
}
