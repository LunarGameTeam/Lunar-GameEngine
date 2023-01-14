#pragma once


#include "core/framework/module.h"
#include "core/reflection/reflection.h"

namespace luna
{


class CORE_API LunaCore : public LObject
{
	RegisterTypeEmbedd(LunaCore, LObject)
public:
	void Run();

public:
	~LunaCore() {};
	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	LModule* LoadModule()
	{
		LType* type = LType::Get<T>();
		LModule* m = type->NewInstance<LModule>();
		return AddModule(m);
	}

	LModule* AddModule(LModule* m);

	void OnRender();

	void OnTick(float delta_time);
	void OnIMGUI();
	void OnFrameBegin(float delta_time);
	void OnFrameEnd(float delta_time);

	void ShutdownModule();
	//TODO 使用自带反射库，而不是RTTI

	template<typename T>
	T* GetModule()
	{		
		return (T*)GetModule(LType::Get<T>());
	}

	LModule* GetModule(LType* type)
	{		
		return mModulesMap[type->GetName()];
	}

	//引擎初始化事件
public:
	SIGNAL(mSubSystemPreInitDoneEvent);
	SIGNAL(mSubSystemInitDoneEvent);
	SIGNAL(mSubSystemPostInitDoneEvent);

	GETTER(float, mFrameDelta, FrameDelta);
	GETTER(float, mFrameRate, FrameRate);
	GET_SET_VALUE(bool, mPendingExit, PendingExit);
	GET_SET_VALUE(float, mRealFrameDelta, ActualFrameDelta);
	GET_SET_VALUE(float, mRealFrameRate, ActualFrameRate);

	static LunaCore* Ins();
	static LunaCore* CreateLunaCore();

private:
	LunaCore();
	bool mPendingExit = false;

	TPPtrArray<LModule> mModules;
	LArray<LModule*> mOrderedModules;
	LUnorderedMap<luna::LString, LModule* > mModulesMap;

	float mRealFrameRate = 0.0f;
	float mRealFrameDelta = 0.0f;
	float mFrameRate = 60.f;
	float mFrameDelta = 1000.f / 60.f;

};

CORE_API extern LunaCore* gEngine;

}
