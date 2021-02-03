#include "engine.h"
#include <chrono>

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

	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;
	auto now = Time::now();
	auto old = Time::now();

	while (!mPendingExit)
	{		
		for (auto& subsystem : mOrderedSubSystems)
		{
			if (subsystem->m_need_tick)
			{
				subsystem->Tick(m_frame_delta / 1000.0f);
			}
		}
		OnRender();
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

void lunaCore::OnRender()
{

}

