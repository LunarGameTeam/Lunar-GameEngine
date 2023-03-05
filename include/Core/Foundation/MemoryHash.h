
#pragma once
#include "core/Foundation/String.h"
#include "core/Foundation/Container.h"
namespace luna
{
	class CORE_API LMemoryHash
	{
	private:
		LArray<uint8_t> mMemory;
		size_t    mHash;
	public:
		LMemoryHash();
		void Reset();
		void GenerateHash();
		void Combine(const uint8_t* memory,size_t size);
		void Combine(const LArray<uint8_t> &memory);
		const size_t& GetHash() const{ return mHash; }
		bool operator==(const LMemoryHash& data) const {
			if(mMemory.size() != data.mMemory.size())
			{
				return false;
			}
			return memcmp(mMemory.data(), data.mMemory.data(), mMemory.size()) == 0;
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
