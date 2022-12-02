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
	void LoadModule()
	{
		T* subSystem = new T();		
		subSystem->SetType(LType::Get<T>());
		LModule* sys = static_cast<LModule*>(subSystem);
		luna::LString name = LType::Get<T>()->GetName();
		sys->OnLoad();
		mModules.PushBack(sys);
		mModulesMap[name] = sys;
		mOrderedModules.push_back(sys);
	}

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
		luna::LString name = LType::Get<T>()->GetName();
		return static_cast<T*>(mModulesMap[name]);
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

	TSubPtrArray<LModule> mModules;
	LVector<LModule*> mOrderedModules;
	LUnorderedMap<luna::LString, LModule* > mModulesMap;

	float mRealFrameRate = 0.0f;
	float mRealFrameDelta = 0.0f;
	float mFrameRate = 60.f;
	float mFrameDelta = 1000.f / 60.f;

};

CORE_API extern LunaCore* gEngine;

}
