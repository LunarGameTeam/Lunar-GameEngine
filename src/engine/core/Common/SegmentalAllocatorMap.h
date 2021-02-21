#pragma once
#include"core/core_module.h"
#include"core/misc/container.h"
#include"core/misc/FastId.h"
namespace luna
{
	struct SegmentalIndex
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
		const size_t GetHash() const
		{
			constexpr size_t hash_helper = unsigned int(-1);
			return segmental_offset * hash_helper + segmental_size;
		}
	};
	class SegmentalAllocatorMap;
	class SegmentalMemberValue
	{
		size_t m_data_offset;
		size_t m_data_size;
		bool if_created;
		SegmentalAllocatorMap* m_allocator;
	public:
		SegmentalMemberValue(SegmentalAllocatorMap* allocator):m_data_offset(0), m_data_size(0), if_created(false), m_allocator(allocator)
		{
		};
		~SegmentalMemberValue()
		{
			m_allocator->ReleaseSegmental(m_data_offset);
		};
		bool CheckIfCreated()
		{
			return if_created;
		}
		LResult Create(const size_t &data_offset, const size_t &data_size)
		{
			if (if_created)
			{
				return g_Succeed;
			}
			LResult check_error;
			m_data_offset = data_offset;
			m_data_size = data_size;
			check_error = OnSegmentalCreated(m_data_offset, m_data_size);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			if_created = true;
			return g_Succeed;
		}
	private:
		virtual LResult OnSegmentalCreated(const size_t &data_offset, const size_t &data_size) = 0;
	};
	class SegmentalAllocatorMap
	{
		size_t m_segmental_size;
		LMap<size_t, SegmentalIndex> m_empty_segmental_by_offset_map;
		LSet<SegmentalIndex> m_empty_segmental_by_size_map;
		LUnorderedSet<SegmentalIndex> m_used_segmental_map;
	public:
		SegmentalAllocatorMap(const size_t &segmental_size):m_segmental_size(segmental_size)
		{
		}
		~SegmentalAllocatorMap()
		{
		}
		LResult Create()
		{
			LResult check_error;
			SegmentalIndex root_segmental;
			root_segmental.segmental_offset = 0;
			root_segmental.segmental_size = m_segmental_size;
			SegmentalMemberValue* root_segmental_data = nullptr;
			check_error = BuildNewSegmentalToMap(m_segmental_size, root_segmental_data);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			m_empty_segmental_by_size_map.insert(root_segmental);
			m_empty_segmental_by_offset_map.insert(LMap<size_t, SegmentalIndex>::value_type(0,root_segmental));
			return g_Succeed;
		}
		LResult AllocateNewSegmental(const size_t &required_segmental_size, SegmentalMemberValue &segmental_value);
		LResult ReleaseSegmental(const size_t &segmental_index, const size_t &segmental_size);
	private:
		virtual LResult BuildNewSegmentalToMap(const size_t &required_segmental_size,SegmentalMemberValue* segmental_value) = 0;
		virtual void ReleaseSegmentalFromMap(const size_t &required_segmental_size) = 0;
	};

	LResult SegmentalAllocatorMap::AllocateNewSegmental(const size_t &required_segmental_size, SegmentalMemberValue &segmental_value)
	{
		LResult check_error;
		if (segmental_value.CheckIfCreated())
		{
			LunarDebugLogError(0, "do not repeate create resource", check_error);
			return check_error;
		}
		//创建一个尺寸变量用于请求指定大小的资源
		SegmentalIndex now_allocate_index;
		now_allocate_index.segmental_offset = static_cast<size_t>(-1);
		now_allocate_index.segmental_size = static_cast<size_t>(required_segmental_size);
		//从资源树里获取一个合适的资源表用于开辟数据，如果没有空闲的表则需要新建一个
		auto min_list_containter = m_empty_segmental_by_size_map.lower_bound(now_allocate_index);
		if (min_list_containter == m_empty_segmental_by_size_map.end())
		{
			LunarDebugLogError(0, "do not have enough empty to create resource", check_error);
			return check_error;
		}
		now_allocate_index.segmental_offset = min_list_containter->segmental_offset;
		check_error = segmental_value.Create(min_list_containter->segmental_offset, required_segmental_size);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		min_list_containter->segmental_offset += required_segmental_size;
		min_list_containter->segmental_size -= required_segmental_size;
		if (min_list_containter->segmental_size == 0)
		{
			m_empty_segmental_map.erase(min_list_containter);
		}
		m_used_segmental_map.insert(now_allocate_index);
		return g_Succeed;
	}
	LResult SegmentalAllocatorMap::ReleaseSegmental(const size_t &segmental_index, const size_t &segmental_size)
	{
		LResult check_error;
		SegmentalIndex now_allocate_index;
		now_allocate_index.index_value = segmental_index;
		now_allocate_index.now_size = segmental_size;
		auto list_containter = m_used_segmental_map.find(now_allocate_index);
		if (list_containter == m_used_segmental_map.end())
		{
			LunarDebugLogError(0, "do not find data to release", check_error);
			return check_error;
		}

	}
}
namespace LContainerPack
{
	size_t hash_value(const luna::SegmentalIndex& key)
	{
		return key.GetHash();
	}
}