#pragma once

#include "core/private_core.h"
#include "marcos.h"

#include "core/misc/uuid.h"
#include "core/reflection/PancyResourceJsonReflect.h"
#include <memory>
#include "boost/uuid/uuid_generators.hpp"
#include "boost/unordered_set.hpp"

template<typename T>
using LSharedPtr = boost::shared_ptr<T>;
//侵入式智能指针
class LSharedObject
{
public:
	LSharedObject()
	{
	}
	LVector<LSharedObject*> sub_object_list;
	LSharedObject*& AddSubObject();
protected:
	virtual ~LSharedObject()
	{

	};
};
enum class LLoadState
{
	LOAD_STATE_EMPTY = 0,
	LOAD_STATE_LOADING,
	LOAD_STATE_FINISHED
};

class LObject :public LSharedObject
{
public:
	GET_SET_VAULE(LUuid, m_uid, LUuid);
	LObject();
	virtual ~LObject();
	//默认加载资源
	LunarEngine::LResult InitResource();
	//检查资源的加载状态
	const LLoadState &GetLoadState();
	//获取名字
	inline const LunarEngine::LString& GetObjectName()
	{
		return m_Name;
	};
private:
	virtual void CheckIfLoadingStateChanged(LLoadState &m_object_load_state) = 0;
	virtual LunarEngine::LResult InitCommon();
protected:
	LunarEngine::LString m_Name;
	LUuid m_uid;
	LLoadState m_object_load_state;
};



