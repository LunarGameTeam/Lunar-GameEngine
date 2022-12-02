#include "LMemoryHash.h"
namespace luna
{
	LMemoryHash::LMemoryHash()
	{
		m_hash_code = -1;
	}
	void LMemoryHash::Init(const byte* memory_pointer, const size_t memory_length)
	{
		m_memory_pointer.clear();
		m_memory_pointer.insert(m_memory_pointer.end(), memory_pointer, memory_pointer + memory_length);
		m_hash_code = MurmurHash64A(memory_pointer, memory_length, 0);
	}
	void LMemoryHash::Init(const char* memory_pointer, const size_t memory_length)
	{
		m_memory_pointer.clear();
		m_memory_pointer.insert(m_memory_pointer.end(), memory_pointer, memory_pointer + memory_length);
		m_hash_code = MurmurHash64A(memory_pointer, memory_length, 0);
	}
	bool LMemoryHash::operator== (const LMemoryHash& p) const
	{
		size_t memory_size = m_memory_pointer.size();
		if (memory_size != p.GetSize())
		{
			return false;
		}
		int if_succeed = memcmp(m_memory_pointer.data(), p.GetData(), memory_size);
		if (if_succeed == 0)
		{
			return true;
		}
		return false;
	}
}