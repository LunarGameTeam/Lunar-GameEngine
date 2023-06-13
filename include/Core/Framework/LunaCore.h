#pragma once


#include "Core/Framework/Module.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Foundation/Config.h"

namespace luna
{

CORE_API CONFIG_DECLARE(int, Core, LogicFPS, 60);
CORE_API CONFIG_DECLARE(int, Core, RenderFPS, 60);

class CORE_API LunaCore : public LObject
{
	RegisterTypeEmbedd(LunaCore, LObject)
public:
	virtual ~LunaCore() { };

	void Run();
	void MainLoop();

	float GetRenderTickTime()
	{
		return mRenderTickTime;
	}

	float GetLogicTickTime()
	{
		return mLogicTickTime;
	}

	int   mLogicFPS       = 0;
	int   mRenderFPS      = 0;
	float mLogicTickTime  = 0.0;
	float mRenderTickTime = 0.0;

public:
	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	LModule* LoadModule()
	{
		LType* type = LType::Get<T>();
		LModule* m = type->NewInstance<LModule>();
		return AddModule(m);
	}

	LModule* AddModule(LModule* m);

	void     OnRenderTick(float deltaTime);
	void     OnLogicTick(float delta_time);
	void     OnImGUI();

	void     ShutdownModule();

	template<typename T>
	T* GetTModule()
	{		
		return (T*)GetModule(LType::Get<T>());
	}

	LModule* GetModule(LType* type)
	{		
		return mModulesMap[type->GetName()];
	}

	//引擎初始化事件
public:
	void SetRenderFPS(int val)
	{
		assert(val > 0);
		mRenderFrameDelta = 1.0f / val;
	}
	void SetLogicFPS(int val)
	{
		assert(val > 0);
		mLogicFrameDelta = 1.0f / val;
	}
	float GetLogicFrameDelta()
	{
		return mLogicFrameDelta;
	}
	float GetRenderFrameDelta()
	{
		return mRenderFrameDelta;
	}	
	void Exit()
	{
		mPendingExit = true;
	}

	static  LunaCore* Ins();

	LString GetConfig(const LString& key);

	void    SetConfig(const LString& key, const LString& val);

	bool    GetPendingExit() const { return mPendingExit; }
private:
	LunaCore();
	bool                                    mPendingExit      = false;

	TPPtrArray<LModule>                     mModules;
	LArray<LModule*>                        mOrderedModules;
	LUnorderedMap<luna::LString, LModule* > mModulesMap;
	float                                   mLogicFrameDelta  = 0.0;
	float                                   mRenderFrameDelta = 0.0;


};

CORE_API extern LunaCore* gEngine;

}
