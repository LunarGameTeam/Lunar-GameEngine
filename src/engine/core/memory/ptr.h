#pragma once

#include <cstdint>
#include <boost/container_hash/hash.hpp>

#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"

template<typename T>
using SharedPtr = boost::shared_ptr<T>;

template<typename T, typename... Args>
SharedPtr<T> MakeShared(Args && ... args)
{
	return boost::make_shared<T>(args...);
}

//只有继承自SharedObject可以使用该指针
template<typename T>
class Ptr
{
public:
	typedef void (*Deletor)(T* ptr);

	static void default_deletor(T *ptr)
	{
		delete ptr;
	}

	Ptr(Deletor dele = default_deletor):
		_ptr(nullptr)
	{
		
	}

	T &operator*()
	{
		assert(_ptr != nullptr);
		return *(_ptr);
	}

	T *operator->()
	{
		assert(_ptr != nullptr);
		return _ptr;
	}

	Ptr(T *ptr, Deletor dele = default_deletor) :
		_ptr(ptr),
		_deletor(dele)
	{
		if (_ptr)
			_ptr->Inc();
	}
	Ptr &operator=(const Ptr &val)
	{
		//防止自我赋值
		if (this->_ptr == val._ptr)
		{
			return *this;
		}
		if (_ptr)
			_ptr->Dec();
		_ptr = val._ptr;
		_deletor = val._deletor;
		if (_ptr)
			_ptr->Inc();
		return *this;
	}
	Ptr(const Ptr &val )
	{
		_ptr = val._ptr;
		_deletor = val._deletor;
		if(_ptr)
			_ptr->Inc();
	}

	~Ptr()
	{
		if (_ptr && _ptr->Dec() == 0)
		{
			_deletor(_ptr);
		}		
		_ptr = nullptr;
	}

	T *get() {
		return _ptr;
	}


	template<typename T>
	friend bool operator!=(const Ptr<T> &l, const Ptr<T> &r);
	template<typename T>
	friend bool operator>(const Ptr<T> &l, const Ptr<T> &r);
	template<typename T>
	friend bool operator<(const Ptr<T> &l, const Ptr<T> &r);
	template<typename T>
	friend bool operator==(const Ptr<T> &l, const Ptr<T> &r);
	template<typename T>
	friend std::size_t hash_value(const Ptr<T> &v);

private:
	T *_ptr = nullptr;
	Deletor _deletor;
};

template<typename T>
std::size_t hash_value(const Ptr<T> &v)
{
	return boost::hash_value(v._ptr);
}

template<typename T>
bool operator==(const Ptr<T> &l,const Ptr<T> &r)
{
	return l._ptr == r._ptr;
}
template<typename T>
bool operator!=(const Ptr<T> &l, const Ptr<T> &r)
{
	return l._ptr != r._ptr;
}
template<typename T>
bool operator>(const Ptr<T> &l, const Ptr<T> &r)
{
	return l._ptr > r._ptr;
}
template<typename T>
bool operator<(const Ptr<T> &l, const Ptr<T> &r)
{
	return l._ptr < r._ptr;
}