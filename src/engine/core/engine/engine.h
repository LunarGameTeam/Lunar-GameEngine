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
#include "core/misc/delegate.h"

class LObject;


class lunaCore
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


	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	T *GetSubsystem()
	{
		return static_cast<T *>(mSubSystems[typeid(T).name()]);
	}

	bool mPendingExit = false;

	//引擎初始化事件
public:
	SIGNAL_NO_PARAMS(mSubSystemPreInitDoneEvent, void);
	SIGNAL_NO_PARAMS(mSubSystemInitDoneEvent, void);
	SIGNAL_NO_PARAMS(mSubSystemPostInitDoneEvent, void);

private:
	boost::container::vector<SubSystem *> mOrderedSubSystems;
	boost::unordered::unordered_map<const char *, SubSystem * > mSubSystems;
};

extern lunaCore *gEngine;
