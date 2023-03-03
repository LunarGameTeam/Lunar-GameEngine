#pragma once

#include"core/Foundation/MemoryHash.h"
namespace luna
{
	LMemoryHash::LMemoryHash():mMemory(nullptr), mMemorySize(0)
	{
		mHash = size_t(-1);
	}
	
	void LMemoryHash::Reset()
	{
		mMemorySize = 0;
		if (mMemory != nullptr)
		{
			delete[] mMemory;
		}
		mHash = size_t(-1);
	}

	void LMemoryHash::Combine(uint8_t* memory, size_t size)
	{
		size_t newSize = mMemorySize + size;
		uint8_t* newMemory = new uint8_t[newSize];
		memcpy(newMemory, mMemory, mMemorySize);
		memcpy(newMemory + mMemorySize, memory, size);
		Reset();
		mMemorySize = newSize;
		mMemory = newMemory;

		mHash = mStr.Hash();
	}
}
