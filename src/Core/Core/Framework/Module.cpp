#include "Core/Framework/Module.h"
#include "Core/Foundation/Log.h"
#include "Core/Foundation/profile.h"
#include "Core/Framework/LunaCore.h"

#include <imgui.h>
#include <chrono>


namespace luna
{

RegisterTypeEmbedd_Imp(LModule)
{
	cls->Ctor<LModule>();
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
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

void LModule::RenderTick(float delta_time)
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
