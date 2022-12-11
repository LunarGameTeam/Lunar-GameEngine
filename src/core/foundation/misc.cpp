#include "misc.h"

size_t luna::SizeAligned(const size_t &size_in, const size_t &size_aligned_in)
{
	size_t out_size = size_in;
	if (size_in % size_aligned_in != 0)
	{
		auto size_scal = size_in / size_aligned_in;
		out_size = (size_scal + 1) * size_aligned_in;
	}
	return out_size;
}
size_t luna::SizeAligned2Pow(const size_t &size_in, const size_t &size_aligned_in)
{
	return (size_in + (size_aligned_in - 1)) & ~(size_aligned_in - 1);
}