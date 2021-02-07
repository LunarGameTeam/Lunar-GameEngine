#include "sub_system.h"
#include "core/log/log.h"

//没有接入反射，先使用RTTI
bool SubSystem::Init()
{
	//LogVerboseFormat(E_Core, "%s Init", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnInit();
}

bool SubSystem::PreInit()
{
	//LogVerboseFormat(E_Core,"%s PreInit", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnPreInit();
}

bool SubSystem::PostInit()
{
	//LogVerboseFormat(E_Core, "%s PostInit", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnPostInit();
}

bool SubSystem::Shutdown()
{
	LogVerboseFormat(E_Core, "%s Shutdown", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnShutdown();
}

void SubSystem::Tick(float delta_time)
{

}

void SubSystem::OnIMGUI()
{

}

void SubSystem::OnFrame(float delta_time)
{

}

void SubSystem::OnFrameBegin(float delta_time)
{

}

void SubSystem::OnFrameEnd(float delta_time)
{

}


#include <chrono>

CORE_API lunaCore *gEngine = nullptr;

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

void lunaCore::OnRender()
{

}

void lunaCore::OnSubsystemTick(float delta_time)
{

	for (auto &subsystem : mOrderedSubSystems)
	{
		if (subsystem->m_need_tick)
		{
			subsystem->Tick(m_frame_delta / 1000.0f);
		}
	}
}

void lunaCore::OnSubsystemIMGUI()
{

	for (auto& subsystem : mOrderedSubSystems)
	{
		subsystem->OnIMGUI();
	}
}

void lunaCore::OnSubsystemFrameBegin(float delta_time)
{
	for (auto &subsystem : mOrderedSubSystems)
	{
		if (subsystem->m_need_tick)
		{
			subsystem->OnFrameBegin(m_frame_delta);
		}
	}

}

void lunaCore::OnSubsystemFrameEnd(float delta_time)
{

	for (auto &subsystem : mOrderedSubSystems)
	{
		if (subsystem->m_need_tick)
		{
			subsystem->OnFrameEnd(m_frame_delta);
		}
	}
}

