#include "Game/GameModule.h"
#include "Game/Scene.h"
#include "Core/Object/Entity.h"
#include "Graphics/RenderModule.h"

#include "Game/Camera.h"
#include "Core/Object/Transform.h"
#include "imgui.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Game/Light.h" 


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

void GameModule::RemoveScene(Scene* new_scene)
{
	mScenes.Erase(new_scene);
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
		if(it)
			it->Tick(delta_time);
	}
}

void GameModule::RenderTick(float deltaTime)
{
	for (auto& it : mScenes)
	{
		if (it)
			it->RenderTick(deltaTime);
	}
}

void GameModule::OnIMGUI()
{
}

}