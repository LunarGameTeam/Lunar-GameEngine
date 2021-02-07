#include "core/core_module.h"
#include "core/core_configs.h"
#include "core/config/config.h"
#include "core/asset/lasset.h"
#include "core/asset/asset_subsystem.h"
#include "core/file/file_subsystem.h"
#include "window/window_subsystem.h"
#include "core/event/event_subsystem.h"


using namespace luna;

class Application
{

public:
	Application()
	{
		gEngine = new lunaCore();
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

		while (!gEngine->m_pending_exit)
		{
			gEngine->OnSubsystemFrameBegin(gEngine->m_frame_delta);
			gEngine->OnSubsystemTick(gEngine->m_frame_delta);			
			gEngine->OnSubsystemFrameEnd(gEngine->m_frame_delta);
			now = Time::now();
			fsec fs = now - old;
			ms d = std::chrono::duration_cast<ms>(fs);			
			if (d.count() < gEngine->m_frame_delta)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds((int)gEngine->m_frame_delta - d.count()));
			}
			old = now;
		}
	}

	bool mPendingExit = false;

};


int main(int argc, const char* argv[])
{
	Application game;
	gEngine->RegisterSubsystem<FileSubsystem>();
	gEngine->RegisterSubsystem<AssetSubsystem>();
	gEngine->RegisterSubsystem<EventSubsystem>();
	gEngine->RegisterSubsystem<WindowSubsystem>();
	game.Run();
	game.MainLoop();

}