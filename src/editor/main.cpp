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

class EditorApplication
{

public:
	EditorApplication()
	{
		gEngine = new lunaCore();
		gEngine->RegisterSubsystem<FileSubsystem>();
		gEngine->RegisterSubsystem<AssetSubsystem>();
		gEngine->RegisterSubsystem<EventSubsystem>();
		gEngine->RegisterSubsystem<WindowSubsystem>();
		gEngine->RegisterSubsystem<editor::EditorSubsystem>();
	}
	void Run()
	{
		gEngine->Run();
	}
	void MainLoop()
	{
		typedef std::chrono::high_resolution_clock Time;
		typedef std::chrono::milliseconds ms;
		typedef std::chrono::duration<float> fsec;
		auto now = Time::now();
		auto old = Time::now();

		while (!gEngine->GetPendingExit())
		{
			gEngine->OnSubsystemFrameBegin(gEngine->GetFrameDelta());
			gEngine->OnSubsystemTick(gEngine->GetFrameDelta());
			gEngine->OnSubsystemFrameEnd(gEngine->GetFrameDelta());
			now = Time::now();
			fsec fs = now - old;
			ms d = std::chrono::duration_cast<ms>(fs);

			if (d.count() < gEngine->GetFrameDelta())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds((int)gEngine->GetFrameDelta() - d.count()));
			}
			fs = Time::now() - old;
			d = std::chrono::duration_cast<ms>(fs);
			gEngine->SetActualFrameRate(1000 / d.count());
			gEngine->SetActualFrameDelta(d.count());
			old = now;
		}
	}

	bool mPendingExit = false;

};

#undef main

int main(int argc, const char* argv[])
{
	EditorApplication game;
	game.Run();
	game.MainLoop();
	return 1;
}