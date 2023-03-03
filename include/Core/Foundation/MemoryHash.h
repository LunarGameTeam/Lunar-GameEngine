
#pragma once
#include "core/Foundation/String.h"
#include "MurmurHash2.h"
namespace luna
{
	class CORE_API LMemoryHash
	{
	private:
		size_t    mMemorySize;
		uint8_t*  mMemory;
		size_t    mHash;
	public:
		LMemoryHash();
		void Reset();
		void Combine(uint8_t* memory,size_t size);
		const size_t& GetHash() const{ return mHash; }
		bool operator==(const LMemoryHash& data) const {
			return memcmp(mMemory, data.mMemory, mMemorySize) == 0;
		}
	};
}

namespace std
{

template<> struct std::hash<luna::LMemoryHash>
{
	std::size_t operator() (const luna::LMemoryHash& p) const
	{
		return p.GetHash();
	}
};
}
