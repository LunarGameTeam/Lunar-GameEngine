#pragma once

#include "core/private_core.h"
#include "core/object/object.h"


// #define DEPENDENCY_SUBSYSTEM_DECLARE(SubSystemClass,SystemVarName) \
// 	private:\
// 	ptr<SubSystemClass> SystemVarName;
// 
// #define DEPENDENCY_SUBSYSTEM_IMP(SubSystemClass,SystemVarName) \
// 	SystemVarName = gEngine->GetSubsystem<SubSystemClass>();

class CORE_API SubSystem
{
public:
	bool IsInitialized()
	{
		return m_is_initialized;
	}

	bool Init();
	bool PreInit();
	bool PostInit();
	bool Shutdown();

	virtual bool OnPreInit() = 0;
	virtual bool OnPostInit() = 0;
	virtual bool OnInit() = 0;
	virtual bool OnShutdown() = 0;

	virtual void Tick(float delta_time) = 0;

	virtual void OnFrame(float delta_time);
	virtual void OnFrameBegin(float delta_time);
	virtual void OnFrameEnd(float delta_time);

protected:
	bool m_is_initialized = false;
	bool m_need_tick = false;

	friend class lunaCore;
};



#include "boost/unordered/unordered_map.hpp"
#include "boost/container/vector.hpp"

#include "core/subsystem/sub_system.h"
#include "core/misc/signal.h"

class LObject;


class CORE_API lunaCore
{
	
public:
	void Run();

public:

	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	void RegisterSubsystem()
	{
		T *subSystem = new T();
		SubSystem *sub = static_cast<SubSystem *>(subSystem);
		mSubSystems[typeid(T).name()] = sub;
		mOrderedSubSystems.push_back(sub);
	}

	void OnRender();

	void OnSubsystemTick(float delta_time);
	void OnSubsystemFrameBegin(float delta_time);
	void OnSubsystemFrameEnd(float delta_time);

	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	T *GetSubsystem()
	{
		return static_cast<T *>(mSubSystems[typeid(T).name()]);
	}

	//引擎初始化事件
public:
	SIGNAL(mSubSystemPreInitDoneEvent);
	SIGNAL(mSubSystemInitDoneEvent);
	SIGNAL(mSubSystemPostInitDoneEvent);

	void SetPendingExit(bool val) { m_pending_exit = val; }
private:
	bool m_pending_exit = false;
	boost::container::vector<SubSystem *> mOrderedSubSystems;
	boost::unordered::unordered_map<const char *, SubSystem * > mSubSystems;
	float m_frame_rate = 60.f;
	float m_frame_delta = 1000.f / 60.f;

	friend class Application;
};

extern lunaCore *gEngine;
