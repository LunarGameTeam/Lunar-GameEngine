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
#include "core/delegates/delegate.h"

DELEGATE_NO_PARAMS(SubSystemPreInitDone,Object,void)
DELEGATE_NO_PARAMS(SubSystemInitDone, Object, void)
DELEGATE_NO_PARAMS(SubSystemPostInitDone, Object, void)

class LunarEngineCore
{
public:
	void Run();
	void MainLoop();
public:

	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	void RegisterSubsystem()
	{
		Ptr<T> subSystem = CreateObject<T>(nullptr);
		Ptr<SubSystem> sub = static_cast<SubSystem *>(subSystem.get());
		mSubSystems[typeid(T).name()] = sub;
		mOrderedSubSystems.push_back(sub);
	}


	//TODO 使用自带反射库，而不是RTTI
	template<typename T>
	Ptr<T> GetSubsystem()
	{
		return Ptr<T>(static_cast<T *>(mSubSystems[typeid(T).name()].get()));
	}

	bool mPendingExit = false;

	//引擎初始化事件
public:
	SubSystemPreInitDone mSubSystemPreInitDoneEvent;
	SubSystemInitDone mSubSystemInitDoneEvent;
	SubSystemPostInitDone mSubSystemPostInitDoneEvent;

private:
	boost::container::vector< Ptr<SubSystem> > mOrderedSubSystems;
	boost::unordered::unordered_map<const char*, Ptr<SubSystem> > mSubSystems;
};

extern LunarEngineCore* gEngine;