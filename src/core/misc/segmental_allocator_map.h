#pragma once
#include"core/core_library.h"
#include"core/misc/container.h"
#include"core/misc/fast_id.h"
namespace luna
{
	struct CORE_API SegmentalIndex
	{
		size_t segmental_offset;
		size_t segmental_size;
		//重载hash运算符
		bool operator== (const SegmentalIndex& p) const
		{
			if ((segmental_offset == p.segmental_offset) && (segmental_size == segmental_size))
			{
				return true;
			}
			return false;
		}
		//重载hash运算符
		const size_t GetHash() const
		{
			const size_t hash_helper = unsigned int(-1);
			return segmental_offset * hash_helper + segmental_size;
		}
		//重载小于运算符
		bool operator<(const SegmentalIndex& other)  const
		{
			if (segmental_size != other.segmental_size)
			{
				return (segmental_size < other.segmental_size);
			}
			return (segmental_offset < other.segmental_offset);
		}
	};
}
namespace std
{
	template<> struct std::hash<luna::SegmentalIndex>
	{
		std::size_t operator() (const luna::SegmentalIndex& p) const
		{
			return p.GetHash();
		}
	};
}
namespace luna
{
	class CORE_API SegmentalAllocatorMap;
	class CORE_API SegmentalMemberValue
	{
		size_t m_data_offset;
		size_t m_data_size;
		bool if_created;
		SegmentalAllocatorMap* m_allocator;
	public:
		SegmentalMemberValue();
		~SegmentalMemberValue();
		bool CheckIfCreated();
		SegmentalAllocatorMap* GetAllocator()
		{
			return m_allocator;
		}
		bool Create(SegmentalAllocatorMap* allocator, const size_t& data_offset, const size_t& data_size);
	private:
		virtual bool OnSegmentalCreated(const size_t& data_offset, const size_t& data_size) = 0;
	};
	class CORE_API SegmentalAllocatorMap
	{
		size_t m_segmental_size;
		LMap<size_t, SegmentalIndex> m_empty_segmental_by_offset_map;
		LSet<SegmentalIndex> m_empty_segmental_by_size_map;
		LUnorderedSet<SegmentalIndex> m_used_segmental_map;
	public:
		SegmentalAllocatorMap(const size_t& segmental_size);
		~SegmentalAllocatorMap();
		bool Create();
		bool AllocateNewSegmental(const size_t& required_segmental_size, SegmentalMemberValue* segmental_value);
		bool ReleaseSegmental(const size_t& segmental_index, const size_t& segmental_size);
	private:
		virtual bool BuildNewSegmentalToMap(const SegmentalIndex& required_segmental_index, SegmentalMemberValue* segmental_value) = 0;
		virtual bool ReleaseSegmentalFromMap(const SegmentalIndex& required_segmental_index) = 0;
	};
}