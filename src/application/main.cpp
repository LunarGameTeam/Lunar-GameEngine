#include "core/core_module.h"
#include "core/core_configs.h"
#include "render/render_subsystem.h"
#include "core/config/config.h"
#include "window/window_subsystem.h"
int main(int argc, const char* argv[])
{
	gEngine = new LunarEngineCore();
	gEngine->RegisterSubsystem<WindowSubusystem>();
	gEngine->RegisterSubsystem<RenderSubusystem>();
	gEngine->Run();
	gEngine->MainLoop();
}