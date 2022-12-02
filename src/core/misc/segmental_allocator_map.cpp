#include"segmental_allocator_map.h"

using namespace luna;

SegmentalMemberValue::SegmentalMemberValue() :m_data_offset(0), m_data_size(0), if_created(false), m_allocator(nullptr)
{
};
SegmentalMemberValue::~SegmentalMemberValue()
{
	m_allocator->ReleaseSegmental(m_data_offset, m_data_size);
};
bool SegmentalMemberValue::CheckIfCreated()
{
	return if_created;
}
bool SegmentalMemberValue::Create(SegmentalAllocatorMap* allocator, const size_t& data_offset, const size_t& data_size)
{
	if (if_created)
	{
		return true;
	}
	m_data_offset = data_offset;
	m_data_size = data_size;
	m_allocator = allocator;
	if(!OnSegmentalCreated(m_data_offset, m_data_size))
		return false;
	if_created = true;
	return true;
}

SegmentalAllocatorMap::SegmentalAllocatorMap(const size_t& segmental_size) :m_segmental_size(segmental_size)
{
}
SegmentalAllocatorMap::~SegmentalAllocatorMap()
{
}
bool SegmentalAllocatorMap::Create()
{
	SegmentalIndex root_segmental;
	root_segmental.segmental_offset = 0;
	root_segmental.segmental_size = m_segmental_size;
	m_empty_segmental_by_size_map.insert(root_segmental);
	m_empty_segmental_by_offset_map.insert(LPair<size_t, SegmentalIndex>(0, root_segmental));
	return true;
}

bool SegmentalAllocatorMap::AllocateNewSegmental(const size_t& required_segmental_size, SegmentalMemberValue* segmental_value)
{
	bool check_error = true;
	if (segmental_value->CheckIfCreated())
	{
		LogError("Device", "do not repeate create resource");
		return false;
	}
	//创建一个尺寸变量用于请求指定大小的资源
	SegmentalIndex now_allocate_index;
	now_allocate_index.segmental_offset = 0;
	now_allocate_index.segmental_size = static_cast<size_t>(required_segmental_size);
	//从资源树里获取一个合适的资源表用于开辟数据，如果没有空闲的表则需要新建一个
	auto min_list_containter = m_empty_segmental_by_size_map.lower_bound(now_allocate_index);
	if (min_list_containter == m_empty_segmental_by_size_map.end())
	{
		LogError("Device", "do not have enough empty to create resource");
		return false;
	}
	SegmentalIndex middle_empty_size = *min_list_containter;
	//获取完后立即从表单中删除原始资源
	m_empty_segmental_by_size_map.erase(min_list_containter);
	//从获取的空白资源里面，申请一个需要的资源
	now_allocate_index.segmental_offset = middle_empty_size.segmental_offset;
	check_error = segmental_value->Create(this, middle_empty_size.segmental_offset, required_segmental_size);
	if (!check_error)
	{
		return check_error;
	}
	//这里需要同时维护两个表中的数据，因此需要将另一个表单中的数据也删除
	auto min_list_containter_by_offset =  m_empty_segmental_by_offset_map.find(middle_empty_size.segmental_offset);
	if (min_list_containter_by_offset == m_empty_segmental_by_offset_map.end())
	{
		LogError("Device", "could not find required empty member in offset")
		return false;
	}
	m_empty_segmental_by_offset_map.erase(min_list_containter_by_offset);
	//全部数据处理完毕后，更新红黑树，先计算开辟完请求资源后，剩余了多少空间
	middle_empty_size.segmental_offset += required_segmental_size;
	middle_empty_size.segmental_size -= required_segmental_size;
	//剩余空间不为0则需要把剩余的空间还给分配器
	if (middle_empty_size.segmental_size != 0)
	{
		m_empty_segmental_by_size_map.insert(middle_empty_size);
		m_empty_segmental_by_offset_map.insert(LPair<size_t, SegmentalIndex>(middle_empty_size.segmental_offset, middle_empty_size));
	}
	//记录下当前开辟的空间信息
	m_used_segmental_map.insert(now_allocate_index);
	//执行子类的数据开辟
	check_error = BuildNewSegmentalToMap(now_allocate_index, segmental_value);
	if (!check_error)
	{
		return check_error;
	}
	return true;
}
bool SegmentalAllocatorMap::ReleaseSegmental(const size_t& segmental_index, const size_t& segmental_size)
{
	bool check_error = true;
	SegmentalIndex now_allocate_index;
	now_allocate_index.segmental_offset = segmental_index;
	now_allocate_index.segmental_size = segmental_size;
	auto list_containter = m_used_segmental_map.find(now_allocate_index);
	if (list_containter == m_used_segmental_map.end())
	{
		LogError("Device", "do not find data to release");
		return false;
	}
	//执行子类的数据删除
	check_error = ReleaseSegmentalFromMap(now_allocate_index);
	if (!check_error)
	{
		return check_error;
	}
	//接下来需要检查当前段数据是否有前后接壤的空白段数据
	SegmentalIndex segmental_empty_before;
	SegmentalIndex segmental_empty_after;
	//先检查前置空白段
	auto before_list_containter = m_empty_segmental_by_offset_map.lower_bound(segmental_index);
	
	segmental_empty_before.segmental_offset = -1;
	segmental_empty_before.segmental_size = 0;
	if (before_list_containter != m_empty_segmental_by_offset_map.begin())
	{
		before_list_containter--;
		//这里找到前置段之后，需要严格检查前后两个段是否匹配
		if (before_list_containter->second.segmental_offset + before_list_containter->second.segmental_size == segmental_index)
		{
			segmental_empty_before = before_list_containter->second;
			m_empty_segmental_by_offset_map.erase(before_list_containter);
			m_empty_segmental_by_size_map.erase(segmental_empty_before);
		}
	}
	//再检查后置空白段
	size_t back_offset = segmental_index + segmental_size;
	auto back_list_containter = m_empty_segmental_by_offset_map.find(back_offset);
	if (back_list_containter == m_empty_segmental_by_offset_map.end())
	{
		segmental_empty_after.segmental_offset = -1;
		segmental_empty_after.segmental_size = 0;
	}
	else
	{
		segmental_empty_after = back_list_containter->second;
		m_empty_segmental_by_offset_map.erase(back_list_containter);
		m_empty_segmental_by_size_map.erase(segmental_empty_after);
	}
	//根据原始的段结果合并空白段信息
	SegmentalIndex new_empty_index;
	new_empty_index.segmental_offset = segmental_index;
	new_empty_index.segmental_size = segmental_size;
	if (segmental_empty_before.segmental_size != 0)
	{
		new_empty_index.segmental_offset = segmental_empty_before.segmental_offset;
		new_empty_index.segmental_size += segmental_empty_before.segmental_size;
	}
	if (segmental_empty_after.segmental_size != 0)
	{
		new_empty_index.segmental_size += segmental_empty_after.segmental_size;
	}
	m_empty_segmental_by_offset_map.insert(LPair<size_t, SegmentalIndex>(new_empty_index.segmental_offset, new_empty_index));
	m_empty_segmental_by_size_map.insert(new_empty_index);
	m_used_segmental_map.erase(now_allocate_index);
	return true;
}
