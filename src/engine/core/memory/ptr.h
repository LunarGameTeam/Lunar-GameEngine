#pragma once

#include <cstdint>
#include <boost/container_hash/hash.hpp>
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"

template<typename T>
using LSharedPtr = boost::shared_ptr<T>;

template<typename T>
using LWeakPtr = boost::weak_ptr<T>;

class LSharedObject;

//template<typename T>
//using LSharedPtr = boost::shared_ptr<T>;
//

template<typename T, typename... Args>
inline LSharedPtr<T> MakeShared(Args && ... args)
{
	return boost::make_shared<T>(args...);
}
class LPtrBasic
{
	LSharedObject** m_res_pointer;
	LSharedObject* parent_pointer;
public:
	explicit LPtrBasic(LSharedObject* Parent);
	LPtrBasic(const LPtrBasic&) = delete;
	void operator=(const LPtrBasic&) = delete;
	virtual ~LPtrBasic();
	LSharedObject* Get()
	{
		return *m_res_pointer;
	}
	void operator=(LSharedObject* val);
protected:
	void SetValueToData(LSharedObject* val);
};
template<typename ObjectType>
class LPtr: public LPtrBasic
{
	ObjectType* m_res_pointer;
public:
	explicit LPtr(LSharedObject* Parent) :LPtrBasic(Parent)
	{
	}
	LPtr(const LPtr&) = delete;
	LPtr operator=(const LPtr&) = delete;
	~LPtr()
	{
	};
	ObjectType* Get()
	{
		return m_res_pointer;
	};
	void operator=(ObjectType* val)
	{
		SetValueToData(val);
		m_res_pointer = val;
	}
};

class WeakPtr
{

};