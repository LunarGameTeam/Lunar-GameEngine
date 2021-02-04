/*!
 * \file engine.h
 * \date 2020/08/09 10:46
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief 引擎主文件
 *
 * TODO: long description
 *
 * \note
*/
#pragma once
#include "boost/unordered/unordered_map.hpp"
#include "boost/container/vector.hpp"

#include "core/subsystem/sub_system.h"
#include "core/misc/signal.h"

class LObject;


class CORE_API lunaCore
{
public:
	void Run();
	void MainLoop();
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

private:
	boost::container::vector<SubSystem *> mOrderedSubSystems;
	boost::unordered::unordered_map<const char *, SubSystem * > mSubSystems;
	float m_frame_rate = 60.f;
	float m_frame_delta = 1000.f / 60.f;
};

extern lunaCore *gEngine;
