#include "private_core.h"
#include "core/asset/asset_subsystem.h"
#include "core/file/file_subsystem.h"
#include "core/event/event_subsystem.h"
#include "core/config/config_manager.h"
#include "core/subsystem/sub_system.h"


namespace luna
{

class LApp
{
public:
	
	LApp()
	{		
		gEngine = new lunaCore();
		gEngine->RegisterSubsystem<FileSubsystem>();
		gEngine->RegisterSubsystem<AssetSubsystem>();
		gEngine->RegisterSubsystem<EventSubsystem>();
	}
	virtual ~LApp()
	{
	}

	virtual void Run()
	{
		gEngine->Run();
		Init();
	}
	virtual void Init()
	{

	}
	virtual void MainLoop()
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
			gEngine->SetActualFrameRate(1000.f / d.count());
			gEngine->SetActualFrameDelta(d.count());
			old = now;
		}
	}
};


}