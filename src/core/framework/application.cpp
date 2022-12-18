#include "core/framework/application.h"

#include <thread>
namespace luna
{
CORE_API LApplication* s_app = nullptr;

RegisterTypeEmbedd_Imp(LApplication)
{
	cls->Ctor<LApplication>();
	cls->Binding<LApplication>();
	cls->BindingMethod<&LApplication::Run>("run");
	cls->BindingMethod<&LApplication::MainLoop>("main_loop");
	cls->BindingMethod<&LApplication::Instance>("instance");	
	
	BindingModule::Get("luna")->AddType(cls);
}

LApplication::LApplication()
{
	LunaCore::Ins();
}

LApplication* LApplication::Instance()
{
	if (s_app == nullptr)
		s_app = NewObject<LApplication>(StaticType());
	return s_app;
}

void LApplication::Run()
{
	gEngine->Run();
	Init();
}

void LApplication::MainLoop()
{
	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;
	auto now = Time::now();
	auto old = Time::now();

	while (!gEngine->GetPendingExit())
	{
		gEngine->OnFrameBegin(gEngine->GetFrameDelta());
		gEngine->OnTick(gEngine->GetFrameDelta());
		gEngine->OnFrameEnd(gEngine->GetFrameDelta());
		now = Time::now();
		fsec fs = now - old;
		ms d = std::chrono::duration_cast<ms>(fs);

		if (d.count() < gEngine->GetFrameDelta())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int)gEngine->GetFrameDelta() - d.count()));
		}
		fs = Time::now() - old;
		d = std::chrono::duration_cast<ms>(fs);
		gEngine->SetActualFrameRate(1000.f / d.count());
		gEngine->SetActualFrameDelta((float)d.count());
		old = now;
		FrameMark;
	}
	gEngine->ShutdownModule();
}

}
