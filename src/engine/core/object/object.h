#pragma once

#include "core/private_core.h"
#include "marcos.h"
#include "core/memory/ptr.h"
#include "core/misc/uuid.h"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/unordered_set.hpp"

//侵入式智能指针
class SharedObject
{
public:

	typedef void (*Deletor)(SharedObject *ptr);

	SharedObject() :m_ref(0)
	{
	}
	uint32_t Count()
	{
		return m_ref;
	}
	uint32_t Inc()
	{
		return ++m_ref;
	}
	uint32_t Dec()
	{
		m_ref--;
		if (m_ref == 0)		
			return 0;		
		return m_ref;
	}


protected:
	virtual ~SharedObject()
	{

	};

private:
	uint32_t m_ref;
};

class Object : public SharedObject
{
public:
	GET_SET_VAULE(LUuid, m_uid, LUuid);
	
	virtual ~Object();

	static UnorderedSet<Ptr<Object>> s_root_objects;

protected:
	Object();	
	template<typename T, typename... Args>
	friend Ptr<T> CreateObject(Object *owner, Args &&... arg);
	template<typename T>
	friend void DeleteObject(T* ptr);

protected:
	Ptr<Object> _owner;
	UnorderedSet< Ptr<Object> > _subobjects;

	LunarEngine::LString m_Name;

	LUuid m_uid;

};

template<typename T>
void DeleteObject(T* ptr)
{
	//ptr->PreDestroy();
	if(ptr->_owner.get() == nullptr)
		Object::s_root_objects.erase(Ptr<Object>(ptr));
	delete ptr;
}

template<typename T,typename... Args>
Ptr<T> CreateObject(Object *owner, Args&&... arg)
{
	T *res = new T(arg...);
	Ptr<T> ptr(res, DeleteObject<T>);
	if (owner == nullptr)
		Object::s_root_objects.emplace(static_cast<Object*>(res));
	else
		owner->_subobjects.emplace(static_cast<Object*>(res));
	res->_owner = owner;
	//res->OnCreate();
	res->m_Name = "Object";

	return ptr;
}


