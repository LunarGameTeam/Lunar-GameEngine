#include "platform_module.h"

namespace luna
{


RegisterTypeEmbedd_Imp(PlatformModule)
{	
	cls->Binding<PlatformModule>();
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

void luna::PlatformModule::Tick(float delta_time)
{
	LModule::Tick(delta_time);
}

}