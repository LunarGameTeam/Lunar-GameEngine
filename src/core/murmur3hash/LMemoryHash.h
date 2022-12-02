#pragma once
#include <core/misc/container.h>
#include <core/misc/string.h>
#include "MurmurHash2.h"
namespace luna
{
	class CORE_API LMemoryHash
	{
		LVector<byte> m_memory_pointer;
		size_t m_hash_code;
	public:
		LMemoryHash();
		void Init(const byte* memory_pointer, const size_t memory_length);
		void Init(const char* memory_pointer, const size_t memory_length);
		inline const byte* GetData() const
		{
			return m_memory_pointer.data();
		};
		inline const size_t GetSize() const
		{
			return m_memory_pointer.size();
		};
		bool operator== (const LMemoryHash& p) const;
		inline size_t GetHashCode() const
		{
			return m_hash_code;
		}
	};
}
namespace std
{
	template<> struct std::hash<luna::LMemoryHash>
	{
		std::size_t operator() (const luna::LMemoryHash& p) const
		{
			return p.GetHashCode();
		}
	};
}