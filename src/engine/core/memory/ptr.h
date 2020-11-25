#pragma once

#include <cstdint>
#include <boost/container_hash/hash.hpp>
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
class LSharedObject;

//template<typename T>
//using LSharedPtr = boost::shared_ptr<T>;
//
//template<typename T, typename... Args>
//LSharedPtr<T> MakeShared(Args && ... args)
//{
//	return boost::make_shared<T>(args...);
//}
class LPtr
{
	LSharedObject** m_res_pointer;
public:
	LPtr(LSharedObject* Parent);
	LPtr(const LPtr& copy);
	~LPtr();
	LSharedObject* Get()
	{
		return *m_res_pointer;
	}
	LPtr& operator=(const LPtr& val);
	LPtr& operator=(LSharedObject* val);
};