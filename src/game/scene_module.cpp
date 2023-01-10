#include "scene_module.h"
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

SceneModule* sSceneModule = nullptr;

CONFIG_DECLARE(LString, Start, DefaultScene, "");
CONFIG_IMPLEMENT(LString, Start, DefaultScene, "");

RegisterTypeEmbedd_Imp(SceneModule)
{
	cls->Binding<SceneModule>();

	cls->BindingMethod<&SceneModule::BindingAddScene>("add_scene")
		.Doc("def add_scene(self, new_scene: Scene*) -> T:\n\tpass\n");

	BindingModule::Get("luna")->AddType(cls);
}

void SceneModule::BindingAddScene(Scene* newScene)
{
	mScenes.PushBack(newScene);	
}

SceneModule::SceneModule():mScenes(this)
{
	sSceneModule = this;
}

bool SceneModule::OnLoad()
{
	return true;
}

bool SceneModule::OnInit()
{
	mNeedTick = true;
	return true;
}

bool SceneModule::OnShutdown()
{
	return true;
}

void SceneModule::Tick(float delta_time)
{
	for (auto& it : mScenes)
	{
		it->Tick(delta_time);
	}
}

void SceneModule::OnIMGUI()
{
}

}