#pragma once

#include <cstdint>
#include "core/private_core.h"

namespace luna
{

template<typename T>
class CORE_API Singleton
{
private:
	Singleton()
	{
	}
public:
	T &instance() {
		static T t;
		return t;
	}

};

class CORE_API NoCopy
{
protected:
	NoCopy() {};
private:
	NoCopy(const NoCopy &) = delete;
	NoCopy &operator=(const NoCopy &) = delete;
};

	size_t SizeAligned(const size_t& size_in, const size_t& size_aligned_in);
	size_t SizeAligned2Pow(const size_t& size_in, const size_t& size_aligned_in);
	const static size_t CommonSize1K = 1024;
	const static size_t CommonSize64K = 65536;
	const static size_t CommonSize128K = 131072;
	const static size_t CommonSize256K = 262144;
}
