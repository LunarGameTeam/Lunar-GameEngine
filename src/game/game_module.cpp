#include "game_module.h"
#include "game/scene.h"
#include "core/object/entity.h"
#include "render/render_module.h"

#include "game/camera.h"
#include "core/object/transform.h"
#include "imgui.h"
#include "render/renderer/render_scene.h"
#include "game/light.h"
namespace luna
{

GameModule* sGameModule = nullptr;

CONFIG_DECLARE(LString, Start, DefaultScene, "");
CONFIG_IMPLEMENT(LString, Start, DefaultScene, "");

RegisterTypeEmbedd_Imp(GameModule)
{
	cls->Ctor<GameModule>();
	cls->Binding<GameModule>();

	cls->BindingMethod<&GameModule::BindingAddScene>("add_scene")
		.Doc("def add_scene(self, new_scene: Scene*) -> T:\n\tpass\n");

	BindingModule::Get("luna")->AddType(cls);
}

void GameModule::BindingAddScene(Scene* newScene)
{
	mScenes.PushBack(newScene);	
}

GameModule::GameModule():mScenes(this)
{
	sGameModule = this;
}

bool GameModule::OnLoad()
{
	return true;
}

bool GameModule::OnInit()
{
	mNeedTick = true;
	return true;
}

bool GameModule::OnShutdown()
{
	return true;
}

void GameModule::Tick(float delta_time)
{
	for (auto& it : mScenes)
	{
		it->Tick(delta_time);
	}
}

void GameModule::OnIMGUI()
{
}

}