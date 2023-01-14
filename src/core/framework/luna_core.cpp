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
		gEngine = CreateLunaCore();
	return gEngine;
}

LModule* PyGetModule(LType* type) 
{ 
	return gEngine->GetModule(type); 
};

LModule* PyAddModule(LModule* m)
{
	return gEngine->AddModule(m);
};


LString PyGetConfig(const LString& key)
{
	return ConfigLoader::instance().GetValue(key);
};

void PySetConfig(const LString& key, const LString& val)
{
	ConfigLoader::instance().SetValue(key, val);
};

void Exit()
{
	gEngine->SetPendingExit(true);
}


LunaCore* LunaCore::CreateLunaCore()
{
	assert(gEngine == nullptr);
	//唯一的单例
	gEngine = new LunaCore();
	gEngine->LoadModule<PlatformModule>();
	BindingModule::Luna()->AddMethod<&PyGetModule>("get_module").ml_doc = LString::MakeStatic("def get_module(t: Type[T]) -> T:\n\tpass\n");
	BindingModule::Luna()->AddMethod<&PyAddModule>("add_module").ml_doc = LString::MakeStatic("def load_module(t: Type[T]) -> T:\n\tpass\n");

	BindingModule::Luna()->AddMethod<&Exit>("exit");

	BindingModule::Luna()->AddMethod<&PyGetConfig>("get_config");
	BindingModule::Luna()->AddMethod<&PySetConfig>("set_config");
	return gEngine;
}

LunaCore::LunaCore() : mModules(this)
{

}

void LunaCore::Run()
{

}

LModule* LunaCore::AddModule(LModule* m)
{
	LType* type = m->GetClass();
	luna::LString name = type->GetName();

	if (type->IsNativeType())
		m->OnLoad();
	else
		m->InvokeBinding("on_load");

	mModules.PushBack(m);
	mModulesMap[name] = m;
	mOrderedModules.push_back(m);
	return m;
}

void LunaCore::OnRender()
{

}

void LunaCore::OnTick(float delta_time)
{
	ZoneScoped;
	float logicDelta = mFrameDelta / 1000.0f;
	for (LModule* it : mOrderedModules)
	{
		const char* name = it->GetName();
		if (it->mNeedTick)
		{
			if (it->GetClass()->IsNativeType())
			{
				it->Tick(logicDelta);
			}
			else
			{
				it->InvokeBinding("on_tick", logicDelta);
			}			
		}
	}
}

void LunaCore::OnIMGUI()
{
	for (auto& it : mOrderedModules)
	{
		if (it->GetClass()->IsNativeType())
		{
			it->OnIMGUI();
		}
		else
		{
			it->InvokeBinding("on_imgui");
		}
	}
	
}

void LunaCore::OnFrameBegin(float delta_time)
{
	for (LModule* it : mOrderedModules)
	{
		if (!it->mIsInitialized)
		{
			if (it->GetClass()->IsNativeType())
			{
				it->OnInit();
			}
			else
			{
				it->InvokeBinding("on_init");
			}
			it->mIsInitialized = true;
		}
	}
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