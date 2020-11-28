#pragma once

#include "marcos.h"

#include "core/misc/uuid.h"
#include "core/reflection/reflection.h"
#include "core/misc/container.h"
#include <memory>

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
	luna::LResult InitResource();
	//检查资源的加载状态
	const LLoadState &GetLoadState();
	//获取名字
	inline const luna::LString& GetObjectName()
	{
		return m_Name;
	};
private:
	virtual void CheckIfLoadingStateChanged(LLoadState &m_object_load_state) = 0;
	virtual luna::LResult InitCommon();
protected:
	luna::LString m_Name;
	LUuid m_uid;
	LLoadState m_object_load_state;
};



