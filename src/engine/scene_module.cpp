#include "scene_module.h"
#include "engine/scene.h"
#include "core/object/entity.h"
#include "render/render_module.h"

#include "engine/camera.h"
#include "core/object/transform.h"
#include "imgui.h"
#include "render/renderer/render_scene.h"
#include "engine/light.h"
namespace luna
{

SceneModule* sSceneModule = nullptr;

RegisterTypeEmbedd_Imp(SceneModule)
{
	cls->Binding<SceneModule>();

	cls->BindingMethod<&SceneModule::BindingAddScene>("add_scene")
		.Doc("def add_scene(self, new_scene: Scene*) -> T:\n\tpass\n");

	LBindingModule::Get("luna")->AddType(cls);
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