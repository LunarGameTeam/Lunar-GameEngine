#pragma once

#include <cstdint>


namespace luna
{

//初步测试在多DLL下这样实现没有多份实例的bug
template<typename T>
class Singleton
{
public:
	static T* t;
	virtual ~Singleton() = default;
	Singleton(const Singleton &) = delete;
	Singleton(const Singleton &&) = delete;
	Singleton &operator=(const Singleton &) = delete;

	static T &instance() 
	{
		if (!t)
			t = new T();
		return *t;
	}
protected:
	Singleton() = default;
};

template<typename T>
T *Singleton<T>::t = nullptr;

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
