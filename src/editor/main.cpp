#include "core/core_module.h"
#include "core/core_configs.h"
#include "core/config/config.h"
#include "core/asset/lasset.h"
#include "core/asset/asset_subsystem.h"
#include "core/file/file_subsystem.h"
#include "window/window_subsystem.h"
#include "core/event/event_subsystem.h"
#include "editor_subsystem.h"


using namespace luna;

class EditorApp
{

public:
	EditorApp()
	{
		gEngine->RegisterSubsystem<WindowSubsystem>();
		gEngine->RegisterSubsystem<editor::EditorSubsystem>();
	}
};

#undef main

int main(int argc, const char* argv[])
{
	EditorApp game;
	game.Run();
	game.MainLoop();
	return 1;
}