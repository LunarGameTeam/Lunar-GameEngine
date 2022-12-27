#include "platform_module.h"

namespace luna
{


RegisterTypeEmbedd_Imp(PlatformModule)
{	
	cls->Binding<PlatformModule>();
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

void luna::PlatformModule::Tick(float delta_time)
{
	LModule::Tick(delta_time);
}

}