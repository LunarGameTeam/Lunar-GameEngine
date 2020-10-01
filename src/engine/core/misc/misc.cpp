#include "misc.h"

uint64_t LunarEngine::SizeAligned(const uint64_t& size_in, const uint64_t& size_aligned_in)
{
	uint64_t out_size = size_in;
	if (size_in % size_aligned_in != 0)
	{
		auto size_scal = size_in / size_aligned_in;
		out_size = (size_scal + 1) * size_aligned_in;
	}
	return out_size;
}