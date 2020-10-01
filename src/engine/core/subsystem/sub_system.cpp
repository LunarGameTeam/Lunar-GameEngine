#include "sub_system.h"
#include "core/log/log.h"

//没有接入反射，先使用RTTI
bool SubSystem::Init()
{
	LogVerboseFormat(E_Core, "%s Init", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnInit();
}

bool SubSystem::PreInit()
{
	LogVerboseFormat(E_Core,"%s PreInit", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnPreInit();
}

bool SubSystem::PostInit()
{
	LogVerboseFormat(E_Core, "%s PostInit", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnPostInit();
}

bool SubSystem::Shutdown()
{
	LogVerboseFormat(E_Core, "%s Shutdown", typeid(decltype(*this)).name());
	//TODO 启动前处理，Profile等
	return OnShutdown();
}

void SubSystem::Tick()
{

}

