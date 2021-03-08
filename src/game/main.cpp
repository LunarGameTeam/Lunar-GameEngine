#include "core/core_module.h"
#include "core/core_configs.h"
#include "core/config/config.h"
#include "core/asset/lasset.h"
#include "core/application.h"
#include "core/event/event_subsystem.h"

#include "window/window_subsystem.h"
#include "window/luna_window.h"

#include "world/camera.h"
#include "world/world_subsystem.h"

#include "legacy_render/render_subsystem.h"
#include "legacy_render/interface/i_camera.h"

using namespace luna;

CONFIG_DECLARE(LString, Game, TestConfig, "TestConfigValue");
CONFIG_IMPLEMENT(LString, Game, TestConfig, "TestConfigValue");

class GameApp : public LApp
{
private:
	legacy_render::RenderSubusystem *render_sys;
	WorldSubsystem *world_sys;
	EventSubsystem *event_sys;

public:
	GameApp()
	{
		gEngine->RegisterSubsystem<WindowSubsystem>();
		gEngine->RegisterSubsystem<WorldSubsystem>();
		gEngine->RegisterSubsystem<legacy_render::RenderSubusystem>();

		render_sys = gEngine->GetSubsystem<legacy_render::RenderSubusystem>();
		world_sys = gEngine->GetSubsystem<WorldSubsystem>();
		event_sys = gEngine->GetSubsystem<EventSubsystem>();
	}

	void Init() override
	{
		auto func = boost::bind(&GameApp::OnInputEvent, this, boost::placeholders::_1, boost::placeholders::_2);
		static auto handle = event_sys->OnInput.Bind(func);
		static auto *main_camera = world_sys->GetMainCameraComponent();

		render_sys->SetCamera(main_camera);
	}

private:
	void OnInputEvent(LWindow& window, InputEvent& event)
	{
		static auto *main_camera = world_sys->GetMainCameraComponent();
		static auto *transform = main_camera->GetTransform();
		
		static bool hold = false;
		static float old_x = 0;
		static float old_y = 0;

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
				
				
				auto rota = Eigen::AngleAxisf(delta_y * M_PI, LVector3f::UnitX());
				rotation = rotation * rota;
				rota = Eigen::AngleAxisf(delta_x * M_PI, LVector3f::UnitY());
				rotation = rotation * rota;
				old_x = x;
				old_y = y;
				transform->SetRotation(rotation);
			}
			
			break;
		}
		case EventType::Input_KeyDown:
		{
			LVector3f delta = LVector3f::Zero();
			if (event.code == KeyCode::W)
			{
				delta = rotation * LVector3f(0, 0, 1);
			}
			else if (event.code == KeyCode::A)
			{
				delta = rotation * LVector3f(-1, 0, 0);
			}
			else if (event.code == KeyCode::S)
			{
				delta = rotation * LVector3f(0, 0, -1);

			}
			else if (event.code == KeyCode::D)
			{
				delta = rotation * LVector3f(1, 0, 0);
			}
			pos = pos + delta;
			transform->SetPosition(pos);

			break;
		}
		}
	}
};

#undef main

int main(int argc, const char* argv[])
{
	GameApp game;
	game.Run();
	game.MainLoop();
	return 1;
}