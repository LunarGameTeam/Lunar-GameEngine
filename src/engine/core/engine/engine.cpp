#include "engine.h"

lunaCore* gEngine = nullptr;

void lunaCore::Run()
{
	//PreInit
	for (auto subsytem : mOrderedSubSystems)
	{
		subsytem->PreInit();
	}
	//广播PreInit执行完毕
	mSubSystemPreInitDoneEvent.BroadCast();


	//Init
	for (auto subsytem : mOrderedSubSystems)
	{
		subsytem->Init();
		subsytem->m_is_initialized = true;
	}
	//广播Init执行完毕
	mSubSystemInitDoneEvent.BroadCast();


	//Post Init
	for (auto subsytem : mOrderedSubSystems)
	{
		subsytem->PostInit();
	}
	//广播PostInit执行完毕
	mSubSystemPostInitDoneEvent.BroadCast();

	//SubSystem Tick

}

void lunaCore::MainLoop()
{
	while (!mPendingExit)
	{
		for (auto& subsystem : mOrderedSubSystems)
		{
			if (subsystem->m_need_tick)
			{
				subsystem->Tick();
			}
		}
		if ((GetKeyState(VK_ESCAPE) & 0x8000))
		{
			mPendingExit = true;
		}
	}
}

