#pragma once

#ifndef CORE_API
#ifdef CORE_EXPORT
#define CORE_API __declspec( dllexport )//宏定义
#else
#define CORE_API __declspec( dllimport )
#endif
#endif

#include <cstdint>
#include <memory>

namespace luna
{

//初步测试在多DLL下这样实现没有多份实例的bug
template<typename T>
class Singleton
{
public:
	static std::shared_ptr<T> t;
	virtual ~Singleton() = default;
	Singleton(const Singleton &) = delete;
	Singleton(const Singleton &&) = delete;
	Singleton &operator=(const Singleton &) = delete;

	static T &instance() 
	{
		if (!t)
			t.reset(new T);
		return *(t.get());
	}
protected:
	Singleton() = default;
};

template<typename T>
std::shared_ptr<T> Singleton<T>::t;

//暂时不能用。。。
class CORE_API NoCopy
{
public:
	NoCopy() {};
	virtual ~NoCopy() {};
private:
	NoCopy(const NoCopy &) = delete;
	NoCopy &operator=(const NoCopy &) = delete;
};

size_t CORE_API SizeAligned(const size_t &size_in, const size_t &size_aligned_in);
size_t CORE_API SizeAligned2Pow(const size_t &size_in, const size_t &size_aligned_in);

const static size_t CommonSize1K = 1024;
const static size_t CommonSize64K = 65536;
const static size_t CommonSize128K = 131072;
const static size_t CommonSize256K = 262144;
}
