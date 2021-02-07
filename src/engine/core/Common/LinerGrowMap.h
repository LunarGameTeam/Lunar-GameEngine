#pragma once
#include"core/core_module.h"
#include"core/misc/container.h"
#include"core/misc/FastId.h"
#include "core/memory/ptr.h"
//template<typename IndexValueType>
//LInitNewFastIdClass(LinerGrowIndexMapID, size_t);
namespace luna
{
	template<typename IndexValueType>
	class LLinerGrowMap;
	//该结构用于标记一个资源的附带size的ID，这个ID用于快速找到剩余容量最小的资源表
	template<typename IndexValueType>
	struct LinerGrowIndex
	{
		IndexValueType index_value;
		size_t now_size;
		//重载小于运算符
		bool operator<(const LinerGrowIndex& other)  const
		{
			if (now_size != other.now_size)
			{
				return (now_size < other.now_size);
			}
			return (index_value < other.index_value);
		}
	};
	//该结构用于标记一个资源的全局id，包括其所在表的id以及其在表内的id以及其所包含的资源数量
	template<typename IndexValueType>
	struct ValueMarkIndex
	{
		IndexValueType list_index;
		IndexValueType data_index;
		IndexValueType data_size;
	};
	template<typename IndexValueType>
	class LinerGrownResourceData
	{
		bool m_if_create;
		ValueMarkIndex<IndexValueType> m_data_index;
		LLinerGrowMap<IndexValueType>* m_alloctor_pointer;
	public:
		LinerGrownResourceData()
		{
			m_if_create = false;
			m_alloctor_pointer = nullptr;
		};
		virtual ~LinerGrownResourceData()
		{
			if (m_if_create)
			{
				m_alloctor_pointer->ReleaseDataFromMap(m_data_index);
			}
		};
		bool CheckIfCreated()
		{
			return m_if_create;
		}
		LResult Create(const ValueMarkIndex<IndexValueType>& data_index, LLinerGrowMap<IndexValueType>* alloctor_pointer)
		{
			LResult check_error;
			m_data_index = data_index;
			m_alloctor_pointer = alloctor_pointer;
			check_error = InitResource(data_index);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			m_if_create = true;
			return luna::g_Succeed;
		}
		//禁止拷贝及赋值操作
		LinerGrownResourceData(const LinerGrownResourceData&) = delete;
		void operator=(const LinerGrownResourceData&) = delete;
	protected:
		LLinerGrowMap<IndexValueType>* GetAllocator()
		{
			return m_alloctor_pointer;
		}
	private:
		virtual LResult InitResource(const ValueMarkIndex<IndexValueType>& data_index) = 0;
	};

	//线性增长分配器的每一个表的结构，代表了每次分配器增长的大小
	template<typename IndexValueType>
	class ILinerGrowListMember
	{
		LLinerGrowMap<IndexValueType>* m_controler_map;
	protected:
		IndexValueType m_max_list_size;
		IndexValueType m_list_index;
	public:
		ILinerGrowListMember(const IndexValueType &list_index,const IndexValueType& max_list_size, LLinerGrowMap<IndexValueType>* controler_map)
		{
			m_list_index = list_index;
			m_max_list_size = max_list_size;
			m_controler_map = controler_map;
		};
		virtual ~ILinerGrowListMember()
		{
		};
		LLinerGrowMap<IndexValueType>* GetAllocator()
		{
			return m_controler_map;
		}
		virtual const size_t GetEmptySize() = 0;
		virtual LResult BuildNewValueFromList(const IndexValueType &require_size, LinerGrownResourceData<IndexValueType> &value_data) = 0;
		virtual LResult ReleaseValueFromList(const IndexValueType &value_index) = 0;
		virtual LResult DefragmenteList() = 0;
	};
	template<typename IndexValueType>
	class LinerGrowListMemberOnlyOne : public ILinerGrowListMember<IndexValueType>
	{
		//已经被分配使用的数据的id
		LUnorderedSet<IndexValueType> now_use_data;
		//尚未被分配使用的数据id
		LUnorderedSet<IndexValueType> now_empty_data;
	public:
		LinerGrowListMemberOnlyOne(
			const IndexValueType& list_index,
			const IndexValueType& max_list_size,
			LLinerGrowMap<IndexValueType>* controler_map
		):ILinerGrowListMember<IndexValueType>(list_index,max_list_size, controler_map)
		{
			for (IndexValueType index_now = 0; index_now < max_list_size; ++index_now)
			{
				now_empty_data.insert(index_now);
			}
		};
		virtual ~LinerGrowListMemberOnlyOne()
		{
		}
		const size_t GetEmptySize() override
		{
			return now_empty_data.size();
		}
		LResult BuildNewValueFromList(const IndexValueType& require_size, LinerGrownResourceData<IndexValueType>& value_data) override
		{
			LResult check_error;
			if (value_data.CheckIfCreated())
			{
				LunarDebugLogError(0, "do not repeate create resource", check_error);
				return check_error;
			}
			if (require_size != static_cast<IndexValueType>(1))
			{
				LunarDebugLogError(0, "This List Only Accept Allocate One block", check_error);
				return check_error;
			}
			if (now_empty_data.size() <= 0)
			{
				LunarDebugLogError(0, "list is full,coud not create new value", check_error);
				return check_error;
			}
			//通过检测，为新资源开辟新的ID号
			ValueMarkIndex<IndexValueType> now_value_data_mark;
			now_value_data_mark.list_index = m_list_index;
			now_value_data_mark.data_index = *now_empty_data.begin();
			now_value_data_mark.data_size = require_size;
			//ID号创建完毕后创建新的资源数据
			value_data.Create(now_value_data_mark, GetAllocator());
			check_error = BuildNewValue(now_value_data_mark);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			now_empty_data.erase(now_value_data_mark.data_index);
			now_use_data.insert(now_value_data_mark.data_index);
			return luna::g_Succeed;
		}
		LResult ReleaseValueFromList(const IndexValueType& value_index) override
		{
			LResult check_error;
			if (now_use_data.find(value_index) == now_use_data.end())
			{
				LunarDebugLogError(0, "could not find map value", check_error);
				return check_error;
			}
			check_error = ReleaseValue(value_index);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			now_use_data.erase(value_index);
			now_empty_data.insert(value_index);
			return luna::g_Succeed;
		}
		LResult DefragmenteList()override
		{
			return luna::g_Succeed;
		}
	private:
		virtual LResult BuildNewValue(const ValueMarkIndex<IndexValueType>& value_index) = 0;
		virtual LResult ReleaseValue(const IndexValueType &value_index) = 0;
	};
	
	template<typename IndexValueType>
	class LLinerGrowMap
	{
		//luna::LSafeIndexType::LinerGrowIndexMapIDIndexGennerator id_generator;
		IndexValueType m_max_size_per_list;
		LUnorderedMap<IndexValueType, LinerGrowIndex<IndexValueType>> index_size_map;
		LMap<LinerGrowIndex<IndexValueType>, ILinerGrowListMember<IndexValueType>*> value_data_map;
	public:
		LLinerGrowMap(const IndexValueType& max_size_per_list)
		{
			m_max_size_per_list = max_size_per_list;
		};
		virtual ~LLinerGrowMap()
		{
		}
		luna::LResult AllocatedDataFromMap(const IndexValueType &alloc_require_size, LinerGrownResourceData<IndexValueType>&allocate_data_from_map)
		{
			luna::LResult check_error;
			//创建一个尺寸变量用于请求指定大小的资源
			LinerGrowIndex<IndexValueType> now_allocate_index;
			now_allocate_index.index_value = static_cast<IndexValueType>(-1);
			now_allocate_index.now_size = static_cast<size_t>(alloc_require_size);
			//从资源树里获取一个合适的资源表用于开辟数据，如果没有空闲的表则需要新建一个
			LinerGrowIndex<IndexValueType> now_used_list_id;
			ILinerGrowListMember<IndexValueType>* now_used_list_data = nullptr;
			auto min_list_containter = value_data_map.lower_bound(now_allocate_index);
			if (min_list_containter != value_data_map.end())
			{
				now_used_list_id = min_list_containter->first;
				now_used_list_data = min_list_containter->second;
			}
			else
			{
				ILinerGrowListMember<IndexValueType>* new_list_data;
				now_used_list_id.index_value = GenerateNewListValue();
				check_error = BuildNewListToMap(now_used_list_id.index_value, m_max_size_per_list,new_list_data);
				if (!check_error.m_IsOK)
				{
					return check_error;
				}
				//尝试分配资源
				now_used_list_data = new_list_data;
			}
			//尝试从表单中开辟对应数量的资源
			check_error = now_used_list_data->BuildNewValueFromList(alloc_require_size, allocate_data_from_map);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			//开辟成功后需要调整排序树
			if (min_list_containter != value_data_map.end())
			{
				index_size_map.erase(min_list_containter->first.index_value);
				value_data_map.erase(min_list_containter);
			}
			now_used_list_id.now_size = now_used_list_data->GetEmptySize();
			index_size_map.emplace(now_used_list_id.index_value, now_used_list_id);
			value_data_map.emplace(now_used_list_id, now_used_list_data);
			return luna::g_Succeed;
		}
		void ReleaseDataFromMap(const ValueMarkIndex<IndexValueType> &m_data_index)
		{
			LinerGrowIndex<IndexValueType> now_used_list_id;
			ILinerGrowListMember<IndexValueType>* now_released_data = nullptr;
			auto value_real_index_find = index_size_map.find(m_data_index.list_index);
			if (value_real_index_find != index_size_map.end())
			{
				auto value_data_real_index = value_real_index_find->second;
				ILinerGrowListMember<IndexValueType>* now_released_data = value_data_map[value_data_real_index];
				now_released_data->ReleaseValueFromList(m_data_index.data_index);
				//释放资源后需要刷新红黑树
				index_size_map.erase(m_data_index.list_index);
				value_data_map.erase(value_data_real_index);

				now_used_list_id.index_value = m_data_index.list_index;
				now_used_list_id.now_size = now_released_data->GetEmptySize();

				index_size_map.emplace(m_data_index.list_index, now_used_list_id);
				value_data_map.emplace(now_used_list_id, now_released_data);
			}

		}
	private:
		void ReleaseListFromMap(const IndexValueType& m_data_index)
		{
			auto value_data_real_index = index_size_map.find(m_data_index);
			if (value_data_real_index != index_size_map.end())
			{
				ILinerGrowListMember<IndexValueType>* now_released_data = value_data_map[value_data_real_index->second];
				OnReleaseListFromMap(now_released_data);
			}
			index_size_map.erase(m_data_index);
			value_data_map.erase(value_data_real_index->second);
		}
		virtual luna::LResult BuildNewListToMap(const IndexValueType& list_index, const IndexValueType& max_size_per_list, ILinerGrowListMember<IndexValueType>*& resource_list_pointer) = 0;
		virtual IndexValueType GenerateNewListValue() = 0;
		virtual luna::LResult OnReleaseListFromMap(ILinerGrowListMember<IndexValueType>* resource_list_pointer) = 0;
	protected:
		ILinerGrowListMember<IndexValueType>* GetListData(const IndexValueType& list_index)
		{
			auto list_index_with_size = index_size_map.find(list_index);
			if (list_index_with_size == index_size_map.end())
			{
				luna::LResult error_message;
				LunarDebugLogError(0, "could not find require index from list", error_message);
				return nullptr;
			}
			return value_data_map[list_index_with_size->second];
		};
	};
}
