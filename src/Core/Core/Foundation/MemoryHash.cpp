#pragma once

#include"core/Foundation/MemoryHash.h"
#include "MurmurHash2.h"
namespace luna
{
	LMemoryHash::LMemoryHash()
	{
		Reset();
	}
	
	void LMemoryHash::Reset()
	{
		mMemory.clear();
		mHash = size_t(-1);
	}

	void LMemoryHash::Combine(const uint8_t* memory, size_t size)
	{
		mMemory.insert(mMemory.end(), memory, memory + size);
	}

	void LMemoryHash::Combine(const LArray<uint8_t>& memory)
	{
		mMemory.insert(mMemory.end(), memory.begin(), memory.end());
	}

	void LMemoryHash::GenerateHash()
	{
		mHash = MurmurHash64A(mMemory.data(), mMemory.size(), 0);
	}
}
