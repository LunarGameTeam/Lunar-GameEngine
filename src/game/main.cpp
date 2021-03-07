#include "core/core_module.h"
#include "core/core_configs.h"
#include "core/config/config.h"
#include "core/asset/lasset.h"
#include "core/application.h"
#include "core/event/event_subsystem.h"

#include "window/window_subsystem.h"
#include "window/luna_window.h"

#include "legacy_render/render_subsystem.h"
#include "legacy_render/interface/i_camera.h"

using namespace luna;

CONFIG_DECLARE(LString, Game, TestConfig, "TestConfigValue");
CONFIG_IMPLEMENT(LString, Game, TestConfig, "TestConfigValue");

class GameApp : public LApp
{

public:
	GameApp()
	{
		gEngine->RegisterSubsystem<WindowSubsystem>();
		gEngine->RegisterSubsystem<luna::legacy_render::RenderSubusystem>();
	}

	void Init() override
	{
		static EventSubsystem *event_sys = gEngine->GetSubsystem<EventSubsystem>();
		auto func = boost::bind(&GameApp::OnInputEvent, this, boost::placeholders::_1, boost::placeholders::_2);
		static auto handle = event_sys->OnInput.Bind(func);
	}
private:
	void OnInputEvent(LWindow& window, InputEvent& event)
	{
		static auto *render_sys = gEngine->GetSubsystem<luna::legacy_render::RenderSubusystem>();
		auto *cam = render_sys->GetCamera();
		static bool hold = false;
		static float old_x;
		static float old_y;
		auto pos = cam->GetPosition();
		auto rotation = cam->GetRotation();
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
				cam->SetRotation(rotation);
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
			cam->SetPosition(pos);

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