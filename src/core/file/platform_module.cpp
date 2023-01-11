#include "platform_module.h"

namespace luna
{


CONFIG_IMPLEMENT(LString, Start, DefaultProject, "");

RegisterTypeEmbedd_Imp(PlatformModule)
{
	cls->Ctor<PlatformModule>();
	cls->Binding<PlatformModule>();
	cls->BindingMethod<&Self::SetProjectDir>("set_project_dir");		

	cls->VirtualProperty("project_dir")
		.Getter<&Self::GetProjectDir>()
		.Binding<PlatformModule, LString>();
	cls->VirtualProperty("engine_dir")
		.Getter<&Self::GetEngineDir>()
		.Binding<PlatformModule, LString>();
	BindingModule::Luna()->AddType(cls);
}


CORE_API PlatformModule *sPlatformModule = nullptr;

PlatformModule::PlatformModule()
{
	assert(sPlatformModule == nullptr);
	sPlatformModule = this;
}

bool PlatformModule::OnLoad()
{
#ifdef _WIN64
	mPlatformFile = new WindowsFileManager();
	mPlatformFile->InitFileManager();
#endif
	return true;
}

bool PlatformModule::OnInit()
{
	mPlatformFile->SetProjectDir(Config_DefaultProject.GetValue());
	return true;
}

bool PlatformModule::OnShutdown()
{
	mPlatformFile->DisposeFileManager();
	delete mPlatformFile;
	return true;
}

LString PlatformModule::GetEngineDir()
{
	return mPlatformFile->EngineDir();
}

LString PlatformModule::GetProjectDir()
{
	return mPlatformFile->ProjectDir();
}

void luna::PlatformModule::Tick(float delta_time)
{
	LModule::Tick(delta_time);
}

}