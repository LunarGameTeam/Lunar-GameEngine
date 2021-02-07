#pragma once

#include "core/private_core.h"
#include "core/object/object.h"
#include "core/misc/string.h"
#include "core/misc/misc.h"

// #define DEPENDENCY_SUBSYSTEM_DECLARE(SubSystemClass,SystemVarName) \
// 	private:\
// 	ptr<SubSystemClass> SystemVarName;
// 
// #define DEPENDENCY_SUBSYSTEM_IMP(SubSystemClass,SystemVarName) \
// 	SystemVarName = gEngine->GetSubsystem<SubSystemClass>();

class CORE_API SubSystem : luna::NoCopy
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
	virtual void OnIMGUI();

	virtual void OnFrame(float delta_time);
	virtual void OnFrameBegin(float delta_time);
	virtual void OnFrameEnd(float delta_time);

protected:
	SubSystem() {};
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
		luna::LString name = typeid(T).name();
		mSubSystems[name] = sub;
		mOrderedSubSystems.push_back(sub);
	}

	void OnRender();

	void OnSubsystemTick(float delta_time);
	void OnSubsystemIMGUI();
	void OnSubsystemFrameBegin(float delta_time);
	void OnSubsystemFrameEnd(float delta_time);

	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	T *GetSubsystem()
	{
		luna::LString name = typeid(T).name();
		return static_cast<T *>(mSubSystems[name]);
	}

	//引擎初始化事件
public:
	SIGNAL(mSubSystemPreInitDoneEvent);
	SIGNAL(mSubSystemInitDoneEvent);
	SIGNAL(mSubSystemPostInitDoneEvent);

	GET_SET_VAULE(bool, m_pending_exit, PendingExit);
	GETTER(float, m_frame_delta, FrameDelta);
	GETTER(float, m_frame_rate, FrameRate);
	GET_SET_VAULE(float, m_actual_framte_delta, ActualFrameDelta);
	GET_SET_VAULE(float, m_actual_frame_rate, ActualFrameRate);


private:
	bool m_pending_exit = false;
	boost::container::vector<SubSystem *> mOrderedSubSystems;
	boost::unordered::unordered_map<luna::LString, SubSystem * > mSubSystems;
	float m_actual_frame_rate = 0.0f;
	float m_actual_framte_delta = 0.0f;
	float m_frame_rate = 60.f;
	float m_frame_delta = 1000.f / 60.f;

	friend class Application;
};

CORE_API extern lunaCore *gEngine;
