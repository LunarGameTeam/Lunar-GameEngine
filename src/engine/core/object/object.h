#pragma once

#include "core/private_core.h"
#include "core/misc/uuid.h"
#include "core/reflection/reflection.h"
#include "core/misc/container.h"
#include <memory>

//侵入式智能指针
class CORE_API LSharedObject
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
static LSharedObject* lroot_object;
enum class LLoadState
{
	LOAD_STATE_EMPTY = 0,
	LOAD_STATE_LOADING,
	LOAD_STATE_FINISHED
};
/*!
 * \class Object
 *
 * \brief 
 *
 * \author 
 * 
 */
class CORE_API LObject :public LSharedObject
{
public:
	GET_SET_VALUE(LUuid, m_uid, LUuid);
	LObject();
	virtual ~LObject();
	//获取名字
	inline const luna::LString& GetObjectName()
	{
		return m_Name;
	};
protected:
	luna::LString m_Name;
	LUuid m_uid;
};



