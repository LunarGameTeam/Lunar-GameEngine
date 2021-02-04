#include "core/core_module.h"
#include "core/core_configs.h"
#include "render/render_subsystem.h"
#include "core/config/config.h"
#include "core/asset/lasset.h"
#include "window/window_subsystem.h"

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

		while (!mPendingExit)
		{




			now = Time::now();
			fsec fs = now - old;
			ms d = std::chrono::duration_cast<ms>(fs);
			if (d.count() < m_frame_delta)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds((int)m_frame_delta - d.count()));
			}
			old = now;
		}
	}

	bool mPendingExit = false;

};


int main(int argc, const char* argv[])
{
	Application game;
	gEngine->RegisterSubsystem<WindowSubsystem>();
	gEngine->RegisterSubsystem<RenderSubusystem>();
	game.Run();
	game.MainLoop();

}