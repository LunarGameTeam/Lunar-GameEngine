#include "core/framework/luna_core.h"
#include "core/file/platform_module.h"
#include "imgui.h"

namespace luna
{

RegisterTypeEmbedd_Imp(LunaCore)
{
	//LunaCore 类型对python不可见，请binding到luna模块内
}

LunaCore* gEngine = nullptr;

LunaCore* LunaCore::Ins()
{
	if (gEngine == nullptr)
	{
		gEngine = CreateLunaCore();
	}
	return gEngine;
}

LModule* PyGetModule(LType* type) { return gEngine->GetModule(type); };

LunaCore* LunaCore::CreateLunaCore()
{
	assert(gEngine == nullptr);
	//唯一的单例
	gEngine = new LunaCore();
	gEngine->LoadModule<PlatformModule>();
	BindingModule::Luna()->AddMethod<&PyGetModule>("get_module").ml_doc = LString::MakeStatic("def get_module(self, t: Type[T]) -> T:\n\tpass\n");
	return gEngine;
}

LunaCore::LunaCore() : mModules(this)
{

}

void LunaCore::Run()
{
	for (LModule* it : mOrderedModules)
	{
		it->OnInit();
		it->mIsInitialized = true;
	}
}

void LunaCore::OnRender()
{
}

void LunaCore::OnTick(float delta_time)
{
	ZoneScoped;
	for (LModule* it : mOrderedModules)
	{
		const char* name = it->GetName();
		if (it->mNeedTick)
		{
			it->Tick(mFrameDelta / 1000.0f);
		}
	}
}

void LunaCore::OnIMGUI()
{
	for (auto& it : mOrderedModules)
	{
		it->OnIMGUI();
	}
	
}

void LunaCore::OnFrameBegin(float delta_time)
{
	for (LModule* it : mOrderedModules)
	{
		if (it->mNeedTick)
		{
			it->OnFrameBegin(mFrameDelta);
		}
	}
}

void LunaCore::OnFrameEnd(float delta_time)
{
	for (LModule* it : mOrderedModules)
	{
		if (it->mNeedTick)
		{
			it->OnFrameEnd(mFrameDelta);
		}
	}
}

void LunaCore::ShutdownModule()
{
	for (LModule* it : mOrderedModules)
	{
		it->Shutdown();
	}
}

}