#include "core/core_library.h"
#include "core/core_configs.h"
#include "core/object/entity.h"
#include "core/config/config.h"
#include "core/asset/lasset.h"
#include "core/object/transform.h"
#include "core/asset/asset_subsystem.h"
#include "core/framework/application.h"
#include "core/misc/profile.h"
#include "core/misc/signal.h"
#include "core/event/event_subsystem.h"

#include "window/window_subsystem.h"
#include "window/window.h"

#include "engine/camera.h"
#include "engine/world_subsystem.h"
#include "engine/scene.h"
#include "engine/light.h"

#include <numbers>

#include "render/render_subsystem.h"
#include "render/component/mesh_renderer.h"
#include "render/asset/mesh_asset.h"

#include "render/interface/camera.h"
//#include "render/asset/pipeline_state_asset.h"
#include "render/renderer/render_target.h"

using namespace luna;

class GameApp : public LApplication
{
private:

public:
	GameApp()
	{
		gEngine->LoadModule<WindowSystem>();
		gEngine->LoadModule<WorldSystem>();
		gEngine->LoadModule<render::RenderSystem>();
		gEngine->LoadModule<LuaSystem>();
		gEngine->GetModule<AssetSystem>();
		gEngine->GetModule<render::RenderSystem>();
		gEngine->GetModule<WorldSystem>();
		gEngine->GetModule<EventSystem>();
	}
	Entity *CreateMeshEntity(const LString &mesh_path, const LString &mat_path, const LVector3f &pos = LVector3f(0, 0, 10), const LVector3f &scale = LVector3f(1, 1, 1))
	{
		auto *entity = g_world_sys->GetSceneManager()->MainScene()->CreateEntity("Entity");

		entity->GetTransform()->SetPosition(pos);
		entity->GetTransform()->SetScale(scale);
		entity->GetTransform()->GetMatrix();
		auto *mesh_renderer = entity->AddComponent<render::MeshRenderer>();
		auto box_mesh = g_asset_sys->LoadAsset<render::MeshAsset>(mesh_path);
		auto mat = g_asset_sys->LoadAsset<render::MaterialAsset>(mat_path);
		mesh_renderer->SetMesh(box_mesh);
		mesh_renderer->SetMaterial(mat->CreateInstance());
		return entity;
	}
	void Init() override
	{
		ScopedProfileGuard g;
		render::RenderTarget * rt = TCreateObject<render::RenderTarget>();
		rt->SetScreenRt(true);
		rt->SetWidth(g_window_sys->GetMainWindow()->GetWindowWidth());
		rt->SetHeight(g_window_sys->GetMainWindow()->GetWindowHeight());		
		rt->Update();
		g_render_sys->SetMainRt(rt);

		auto func = std::bind(&GameApp::OnInputEvent, this, std::placeholders::_1, std::placeholders::_2);
		static auto handle = g_event_sys->OnInput.Bind(func);
		CameraComponent *main_camera = nullptr;
		if(true)
		{

			Scene *main_scene = g_world_sys->GetSceneManager()->MainScene();
			auto *entity = main_scene->CreateEntity("MainCamera");
			entity->GetTransform()->SetPosition(LVector3f(0, 0, -2));
			main_camera = entity->AddComponent<CameraComponent>();

			{
				CreateMeshEntity("/assets/built-in/box.fbx", "/assets/built-in/base.mat", LVector3f(3, 3, 3));
			}
			{
				CreateMeshEntity("/assets/built-in/box.fbx", "/assets/built-in/base.mat", LVector3f(0, 2, 2));
			}
			{
				CreateMeshEntity("/assets/built-in/box.fbx", "/assets/built-in/base.mat", LVector3f(-3, 1, 1));
			}
			{
				CreateMeshEntity("/assets/built-in/plane.obj", "/assets/built-in/base.mat", LVector3f(0, -3, 0), LVector3f(10, 1, 10));
			}
			{
				auto *entity = CreateMeshEntity("/assets/built-in/inside_sphere.obj", "/assets/built-in/skybox/skybox.mat", LVector3f(0, 0, 0), LVector3f(100, 100, 100));
				entity->GetComponent<render::MeshRenderer>()->SetCastShadow(false);
			}
			{
				auto *entity = g_world_sys->GetSceneManager()->MainScene()->CreateEntity("Entity");
				entity->GetTransform()->LookAt(LVector3f(0.5, -1, 0.5).normalized());
				auto forward = entity->GetTransform()->ForwardDirection();
				auto *light = entity->AddComponent<luna::DirectionLightComponent>();
			}
			g_asset_sys->SaveAsset(g_world_sys->GetSceneManager()->MainScene(), "/assets/test.scn");
		}
		else
		{
			Scene* scn = g_asset_sys->LoadAsset<Scene>("/assets/test.scn");
			Entity* entity = scn->FindEntity("MainCamera");
			main_camera = entity->GetComponent<CameraComponent>();
		}

		g_world_sys->SetMainCameraComponent(main_camera);
		g_render_sys->SetMainCamera(main_camera);

		//CreateMeshEntity("/assets/built-in/grid.fbx", "/assets/built-in/default_material.mat", LVector3f(0, -2, 0), LVector3f(1, 1, 1));
	}

private:
	void OnInputEvent(LWindow &window, InputEvent &event)
	{
		auto *main_camera = g_world_sys->GetMainCameraComponent();
		auto *transform = main_camera->GetTransform();

		static bool hold = false;
		static float old_x = 0;
		static float old_y = 0;
		static LVector3f delta = LVector3f::Zero();
		auto pos = transform->GetPosition();
		auto rotation = transform->GetRotation();

		switch (event.type)
		{
		case EventType::Input_MousePress:
		{
			old_x = event.x;
			old_y = event.y;
			hold = true;
			break;
		}
		case EventType::Input_MouseRelease:
		{
			hold = false;
			break;
		}
		case EventType::Input_MouseMove:
		{
			if (hold)
			{
				float x = event.x;
				float y = event.y;
				float delta_x = (x - old_x) / 500;
				float delta_y = (y - old_y) / 500;
				auto v = Eigen::AngleAxisf(delta_y * (float)std::numbers::pi, LVector3f::UnitX());
				auto h = Eigen::AngleAxisf(delta_x * (float)std::numbers::pi, LVector3f::UnitY());
				rotation = h * rotation * v;
				old_x = x;
				old_y = y;
				transform->SetRotation(rotation);
			}

			break;
		}
		case EventType::Input_KeyUp:
		{
			if (event.code == KeyCode::W)
			{
				delta.z() -= 1;
			}
			else if (event.code == KeyCode::A)
			{
				delta.x() += 1;
			}
			else if (event.code == KeyCode::S)
			{
				delta.z() += 1;
			}
			else if (event.code == KeyCode::D)
			{
				delta.x() -= 1;
			}
			else if (event.code == KeyCode::Q)
			{
				delta.y() += 1;
			}
			else if (event.code == KeyCode::E)
			{
				delta.y() -= 1;
			}
			if (delta.size() > 0.01)
			{
				LVector3f direction = transform->GetRotation() * delta;
				main_camera->SetFlyDirection(direction);
				main_camera->SetSpeed(10);
			}
			else
				main_camera->SetSpeed(0);
			break;
		}
		case EventType::Input_KeyDown:
		{
			if (event.code == KeyCode::W)
			{
				delta.z() += 1;
			}
			else if (event.code == KeyCode::A)
			{
				delta.x() -= 1;
			}
			else if (event.code == KeyCode::S)
			{
				delta.z() -= 1;
			}
			else if (event.code == KeyCode::D)
			{
				delta.x() += 1;
			}
			else if (event.code == KeyCode::Q)
			{
				delta.y() -= 1;
			}
			else if (event.code == KeyCode::E)
			{
				delta.y() += 1;
			}
			if (delta.size() > 0.01)
			{
				LVector3f direction = transform->GetRotation() * delta;
				main_camera->SetFlyDirection(direction);
				main_camera->SetSpeed(10);
			}
			else
				main_camera->SetSpeed(0);

			break;
		}
		}
	}
};

#undef main

int main()
{
	::GameApp game;
	game.Run();
	game.MainLoop();
	return 1;
}
