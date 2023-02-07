#include "Core/Framework/LunaCore.h"
#include "Core/Platform/PlatformModule.h"
#include "imgui.h"

namespace luna
{

CONFIG_IMPLEMENT(int, Core, LogicFPS, 30);
CONFIG_IMPLEMENT(int, Core, RenderFPS, 30);


LunaCore* gEngine = nullptr;

LModule* PyGetModule(LType* type)
{
	return gEngine->GetModule(type);
}
void PyAddModule(LModule* m)
{
	gEngine->AddModule(m);
}

LString PyGetConfig(const LString& val)
{
	return gEngine->GetConfig(val);
}

void PySetConfig(const LString& key, const LString& val)
{
	gEngine->SetConfig(key, val);
}

RegisterTypeEmbedd_Imp(LunaCore)
{
	cls->Ctor<LunaCore>();
	cls->Binding<LunaCore>();
	cls->BindingMethod<&LunaCore::Run>("run");
	cls->BindingMethod<&LunaCore::MainLoop>("main_loop");
	cls->BindingMethod<&LunaCore::Ins, MethodType::StaticFunction>("instance");
	cls->BindingMethod<&LunaCore::GetLogicTickTime>("get_logic_tick_time");
	cls->BindingMethod<&LunaCore::GetRenderTickTime>("get_render_tick_time");

	cls->BindingMethod<&LunaCore::GetModule>("get_module")
		.Doc("def get_module(t: Type[T]) -> T:\n\tpass\n");
	cls->BindingMethod<&LunaCore::AddModule>("add_module")
		.Doc("def load_module(t: Type[T]) -> T:\n\tpass\n");
	cls->BindingMethod<&LunaCore::Exit>("exit");
	cls->BindingMethod<&LunaCore::GetConfig>("get_config");
	cls->BindingMethod<&LunaCore::SetConfig>("set_config");

	BindingModule::Get("luna")->AddMethod<&PyGetModule>("get_module")
		.ml_doc = LString::MakeStatic("def get_module(t: Type[T]) -> T:\n\tpass\n");
	BindingModule::Get("luna")->AddMethod<&PyAddModule>("add_module")
		.ml_doc = LString::MakeStatic("def add_module(t: luna.LModule):\n\tpass\n");
	BindingModule::Get("luna")->AddMethod<&PyGetConfig>("get_config");
	BindingModule::Get("luna")->AddMethod<&PySetConfig>("set_config");

	BindingModule::Get("luna")->AddType(cls);
}


LunaCore::LunaCore() : mModules(this)
{

}

void LunaCore::Run()
{
}

void LunaCore::MainLoop()
{
	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;
	auto logicNow = Time::now();
	auto renderNow = Time::now();

	mLogicFrameDelta = 1.0f / Config_LogicFPS.GetValue();
	mRenderFrameDelta = 1.0f / Config_RenderFPS.GetValue();

	auto logicPre = Time::now();
	OnLogicTick(mLogicFrameDelta);
	auto renderPrev = Time::now();
	OnRenderTick(mRenderFrameDelta);

	while (!mPendingExit)
	{
		float loopDelta = std::min<float>(mRenderFrameDelta, mLogicFrameDelta);
		auto loopBegin = Time::now();
		logicNow = Time::now();
		{
			fsec fs = loopBegin - logicPre;
			float f = fs.count();
			if (f >= mLogicFrameDelta)
			{
				FrameMark;
				OnLogicTick(mLogicFrameDelta);
				fsec cost = Time::now() - logicNow;
				mLogicTickTime = cost.count();
				logicPre = logicNow;
			}
		}
		renderNow = Time::now();
		fsec fs = renderNow - renderPrev;
		{
			FrameMark;
			OnRenderTick(mRenderFrameDelta);
			fsec cost = Time::now() - renderNow;
			mRenderTickTime = cost.count();
			renderPrev = renderNow;
		}
	}
	ShutdownModule();
}


LunaCore* LunaCore::Ins()
{
	if (gEngine == nullptr)
	{
		assert(gEngine == nullptr);
		//唯一的单例
		gEngine = NewObject< LunaCore>();
		gEngine->LoadModule<PlatformModule>();
	}
	return gEngine;
}


LString LunaCore::GetConfig(const LString& key)
{
	return ConfigLoader::instance().GetValue(key);
};

void LunaCore::SetConfig(const LString& key, const LString& val)
{
	ConfigLoader::instance().SetValue(key, val);
};

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

void LunaCore::OnRenderTick(float renderDelta)
{
	ZoneScoped;
	for (LModule* it : mOrderedModules)
	{		
		const char* name = it->GetName();
		if (it->mNeedRenderTick)
		{
			it->RenderTick(renderDelta);
		}
	}
}

void LunaCore::OnLogicTick(float logicDelta)
{
	ZoneScoped;
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

void LunaCore::OnImGUI()
{
	for (LModule* it : mOrderedModules)
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

void LunaCore::ShutdownModule()
{
	for (LModule* it : mOrderedModules)
	{		
		if(it->GetClass()->IsNativeType())
			it->Shutdown();
		else
			it->InvokeBinding("on_shutdown");			
	}
}

}