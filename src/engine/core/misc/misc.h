#pragma once

#include <cstdint>

namespace luna
{

template<typename T>
class Singleton
{
public:
	T &instance() {
		static T t;
		return t;
	}

};

	size_t SizeAligned(const size_t& size_in, const size_t& size_aligned_in);
	size_t SizeAligned2Pow(const size_t& size_in, const size_t& size_aligned_in);
	const static size_t CommonSize1K = 1024;
	const static size_t CommonSize64K = 65536;
	const static size_t CommonSize128K = 131072;
	const static size_t CommonSize256K = 262144;
}
