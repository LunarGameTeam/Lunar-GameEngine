#include "core/core_module.h"
#include "core/core_configs.h"
#include "core/config/config.h"
#include "core/asset/lasset.h"
#include "core/application.h"
#include "window/window_subsystem.h"
#include "legacy_render/render_subsystem.h"

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
};

#undef main

int main(int argc, const char* argv[])
{
	GameApp game;
	game.Run();
	game.MainLoop();
	return 1;
}