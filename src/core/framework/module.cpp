#include "module.h"
#include "core/log/log.h"
#include "core/misc/profile.h"
#include "core/framework/luna_core.h"

#include <imgui.h>
#include <chrono>


namespace luna
{

RegisterTypeEmbedd_Imp(LModule)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
}

bool LModule::IsInitialized()
{
	return mIsInitialized;
}

bool LModule::Shutdown()
{
	Log("Core", "{0} Shutdown", GetName());
	//TODO 启动前处理，Profile等
	return OnShutdown();
}

void LModule::Tick(float delta_time)
{

}

void LModule::OnIMGUI()
{

}

void LModule::OnFrame(float delta_time)
{
}
void LModule::OnFrameBegin(float delta_time)
{
}

void LModule::OnFrameEnd(float delta_time)
{
}

const char *LModule::GetName()
{
	return GetClass()->GetName();
}

}
