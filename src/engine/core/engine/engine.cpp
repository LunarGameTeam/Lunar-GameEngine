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
		for (auto subsystem : mOrderedSubSystems)
		{
			if (subsystem->mNeedTick)
			{
				subsystem->Tick();
			}
		}
	}
}

