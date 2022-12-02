#pragma once
#include"core/core_library.h"
#include"core/misc/container.h"
#include"core/misc/fast_id.h"
//template<typename IndexValueType>
//LInitNewFastIdClass(LinerGrowIndexMapID, size_t);
namespace luna
{
	template<typename IndexValueType>
	class LLinearGrowMap;
	//该结构用于标记一个资源的附带size的ID，这个ID用于快速找到剩余容量最小的资源表
	template<typename IndexValueType>
	struct LinearGrowIndex
	{
		IndexValueType index_value;
		size_t now_size;
		//重载==运算符
		bool operator== (const LinearGrowIndex& p) const
		{
			if ((index_value == p.index_value) && (now_size == p.now_size))
			{
				return true;
			}
			return false;
		}
		//重载小于运算符
		bool operator<(const LinearGrowIndex& other)  const
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
	class LinearGrownResourceData
	{
		bool m_if_create;
		ValueMarkIndex<IndexValueType> m_data_index;
		LLinearGrowMap<IndexValueType>* m_alloctor_pointer;
	public:
		LinearGrownResourceData()
		{
			m_if_create = false;
			m_alloctor_pointer = nullptr;
		};
		virtual ~LinearGrownResourceData()
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
		bool Create(const ValueMarkIndex<IndexValueType>& data_index, LLinearGrowMap<IndexValueType>* alloctor_pointer)
		{
			bool check_error;
			m_data_index = data_index;
			m_alloctor_pointer = alloctor_pointer;
			check_error = InitResource(data_index);
			if (!check_error)
			{
				return check_error;
			}
			m_if_create = true;
			return true;
		}
		//禁止拷贝及赋值操作
		LinearGrownResourceData(const LinearGrownResourceData&) = delete;
		void operator=(const LinearGrownResourceData&) = delete;
	protected:
		LLinearGrowMap<IndexValueType>* GetAllocator()
		{
			return m_alloctor_pointer;
		}
	private:
		virtual bool InitResource(const ValueMarkIndex<IndexValueType>& data_index) = 0;
	};

	//线性增长分配器的每一个表的结构，代表了每次分配器增长的大小
	template<typename IndexValueType>
	class ILinearGrowListMember
	{
		LLinearGrowMap<IndexValueType>* m_controler_map;
	protected:
		IndexValueType m_max_list_size;
		IndexValueType m_list_index;
	public:
		ILinearGrowListMember(const IndexValueType &list_index,const IndexValueType& max_list_size, LLinearGrowMap<IndexValueType>* controler_map)
		{
			m_list_index = list_index;
			m_max_list_size = max_list_size;
			m_controler_map = controler_map;
		};
		virtual ~ILinearGrowListMember()
		{
		};
		LLinearGrowMap<IndexValueType>* GetAllocator()
		{
			return m_controler_map;
		}
		virtual const size_t GetEmptySize() = 0;
		virtual bool BuildNewValueFromList(const IndexValueType &require_size, LinearGrownResourceData<IndexValueType> *value_data) = 0;
		virtual bool ReleaseValueFromList(const IndexValueType &value_index) = 0;
		virtual bool DefragmenteList() = 0;
	};
	template<typename IndexValueType>
	class LinerGrowListMemberOnlyOne : public ILinearGrowListMember<IndexValueType>
	{
		//已经被分配使用的数据的id
		LUnorderedSet<IndexValueType> now_use_data;
		//尚未被分配使用的数据id
		LUnorderedSet<IndexValueType> now_empty_data;
	public:
		LinerGrowListMemberOnlyOne(
			const IndexValueType& list_index,
			const IndexValueType& max_list_size,
			LLinearGrowMap<IndexValueType>* controler_map
		):ILinearGrowListMember<IndexValueType>(list_index,max_list_size, controler_map)
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
		bool BuildNewValueFromList(const IndexValueType& require_size, LinearGrownResourceData<IndexValueType>* value_data) override
		{
			bool check_error;
			if (value_data->CheckIfCreated())
			{
				LogError("Core", "do not repeate create resource");
				return check_error;
			}
			if (require_size != static_cast<IndexValueType>(1))
			{
				LogError("Core", "This List Only Accept Allocate One block");
				return check_error;
			}
			if (now_empty_data.size() <= 0)
			{
				LogError("Core", "list is full,coud not create new value");
				return check_error;
			}
			//通过检测，为新资源开辟新的ID号
			ValueMarkIndex<IndexValueType> now_value_data_mark;
			now_value_data_mark.list_index = this->m_list_index;
			now_value_data_mark.data_index = *now_empty_data.begin();
			now_value_data_mark.data_size = require_size;
			//ID号创建完毕后创建新的资源数据
			check_error = value_data->Create(now_value_data_mark, this->GetAllocator());
			if (!check_error)
			{
				return check_error;
			}
			check_error = BuildNewValue(now_value_data_mark);
			if (!check_error)
			{
				return check_error;
			}
			now_empty_data.erase(now_value_data_mark.data_index);
			now_use_data.insert(now_value_data_mark.data_index);
			return true;
		}
		bool ReleaseValueFromList(const IndexValueType& value_index) override
		{
			bool check_error;
			if (now_use_data.find(value_index) == now_use_data.end())
			{
				LogError("Core", "could not find map value");
				return check_error;
			}
			check_error = ReleaseValue(value_index);
			if (!check_error)
			{
				return check_error;
			}
			now_use_data.erase(value_index);
			now_empty_data.insert(value_index);
			return true;
		}
		bool DefragmenteList()override
		{
			return true;
		}
	private:
		virtual bool BuildNewValue(const ValueMarkIndex<IndexValueType>& value_index) = 0;
		virtual bool ReleaseValue(const IndexValueType &value_index) = 0;
	};
	template<typename IndexValueType>
	class LinerGrowListMemberCommon : public ILinearGrowListMember<IndexValueType>
	{
		IndexValueType m_tail_pointer_offset;         //当前表已分配指针的偏移
		IndexValueType m_refresh_pointer_offset;      //当前表如果需要一次整理操作，其合理的起始整理位置
		LUnorderedMap<IndexValueType, ValueMarkIndex<IndexValueType>> list_data_map;//每个被分配出来的数据块
	public:
		LinerGrowListMemberCommon(
			const IndexValueType& list_index,
			const IndexValueType& max_list_size,
			LLinearGrowMap<IndexValueType>* controler_map
		) :ILinearGrowListMember<IndexValueType>(list_index, max_list_size, controler_map)
		{
			m_tail_pointer_offset = 0;
			m_refresh_pointer_offset = this->m_max_list_size;
		};
		virtual ~LinerGrowListMemberCommon()
		{
		}
		const size_t GetEmptySize() override
		{
			return this->m_max_list_size - m_tail_pointer_offset;
		}
		bool BuildNewValueFromList(const IndexValueType& require_size, LinearGrownResourceData<IndexValueType>* value_data) override
		{
			bool check_error;
			if (value_data->CheckIfCreated())
			{
				LogError("Core", "do not repeate create resource");
				return false;
			}
			if (require_size > GetEmptySize())
			{
				LogError("Core", "This List is too full to allocate new data");
				return false;
			}
			//通过检测，为新资源开辟新的ID号
			ValueMarkIndex<IndexValueType> now_value_data_mark;
			now_value_data_mark.list_index = this->m_list_index;
			now_value_data_mark.data_index = m_tail_pointer_offset;
			now_value_data_mark.data_size = require_size;
			//ID号创建完毕后创建新的资源数据
			check_error = value_data->Create(now_value_data_mark, this->GetAllocator());
			if (!check_error)
			{
				return check_error;
			}
			check_error = BuildNewValue(now_value_data_mark);
			if (!check_error)
			{
				return check_error;
			}
			m_tail_pointer_offset += require_size;
			list_data_map.insert(std::pair<IndexValueType, ValueMarkIndex<IndexValueType>>(now_value_data_mark.data_index, now_value_data_mark));
			return true;
		}
		bool ReleaseValueFromList(const IndexValueType& value_index) override
		{
			bool check_error;
			if (list_data_map.find(value_index) == list_data_map.end())
			{
				LogError("Core", "could not find map value");
				return false;
			}
			check_error = ReleaseValue(value_index);
			if (!check_error)
			{
				return check_error;
			}
			list_data_map.erase(value_index);
			if (m_refresh_pointer_offset > value_index)
			{
				m_refresh_pointer_offset = value_index;
			}
			return true;
		}
		bool DefragmenteList()override
		{
			//todo:等需要测试描述符页的回收时进行
			return true;
		}
	private:
		virtual bool BuildNewValue(const ValueMarkIndex<IndexValueType>& value_index) = 0;
		virtual bool ReleaseValue(const IndexValueType &value_index) = 0;
	};

	template<typename IndexValueType>
	class LLinearGrowMap
	{
		//luna::LSafeIndexType::LinerGrowIndexMapIDIndexGennerator id_generator;
		IndexValueType m_max_size_per_list;
		LUnorderedMap<IndexValueType, LinearGrowIndex<IndexValueType>> index_size_map;
		LMap<LinearGrowIndex<IndexValueType>, ILinearGrowListMember<IndexValueType>*> value_data_map;
	public:
		LLinearGrowMap(const IndexValueType& max_size_per_list)
		{
			m_max_size_per_list = max_size_per_list;
		};
		virtual ~LLinearGrowMap()
		{
		}
		bool AllocatedDataFromMap(const IndexValueType &alloc_require_size, LinearGrownResourceData<IndexValueType>*allocate_data_from_map)
		{
			bool check_error;
			//创建一个尺寸变量用于请求指定大小的资源
			LinearGrowIndex<IndexValueType> now_allocate_index;
			now_allocate_index.index_value = static_cast<IndexValueType>((std::min)(-1, 0));
			now_allocate_index.now_size = static_cast<size_t>(alloc_require_size);
			//从资源树里获取一个合适的资源表用于开辟数据，如果没有空闲的表则需要新建一个
			LinearGrowIndex<IndexValueType> now_used_list_id;
			ILinearGrowListMember<IndexValueType>* now_used_list_data = nullptr;
			auto min_list_containter = value_data_map.lower_bound(now_allocate_index);
			if (min_list_containter != value_data_map.end())
			{
				now_used_list_id = min_list_containter->first;
				now_used_list_data = min_list_containter->second;
			}
			else
			{
				ILinearGrowListMember<IndexValueType>* new_list_data;
				now_used_list_id.index_value = GenerateNewListValue();
				check_error = BuildNewListToMap(now_used_list_id.index_value, m_max_size_per_list,new_list_data);
				if (!check_error)
				{
					return check_error;
				}
				//尝试分配资源
				now_used_list_data = new_list_data;
			}
			//尝试从表单中开辟对应数量的资源
			check_error = now_used_list_data->BuildNewValueFromList(alloc_require_size, allocate_data_from_map);
			if (!check_error)
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
			return true;
		}
		void ReleaseDataFromMap(const ValueMarkIndex<IndexValueType> &m_data_index)
		{
			LinearGrowIndex<IndexValueType> now_used_list_id;
			ILinearGrowListMember<IndexValueType>* now_released_data = nullptr;
			auto value_real_index_find = index_size_map.find(m_data_index.list_index);
			if (value_real_index_find != index_size_map.end())
			{
				auto value_data_real_index = value_real_index_find->second;
				ILinearGrowListMember<IndexValueType>* now_released_data = value_data_map[value_data_real_index];
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
		inline IndexValueType GetMaxSizePerList()
		{
			return m_max_size_per_list;
		}
	private:
		void ReleaseListFromMap(const IndexValueType& m_data_index)
		{
			auto value_data_real_index = index_size_map.find(m_data_index);
			if (value_data_real_index != index_size_map.end())
			{
				ILinearGrowListMember<IndexValueType>* now_released_data = value_data_map[value_data_real_index->second];
				OnReleaseListFromMap(m_data_index,now_released_data);
				delete now_released_data;
				index_size_map.erase(m_data_index);
				value_data_map.erase(value_data_real_index->second);
			}
			else
			{
				LogError("Core", "could not release memory list from map");
			}
		}
		virtual bool BuildNewListToMap(const IndexValueType& list_index, const IndexValueType& max_size_per_list, ILinearGrowListMember<IndexValueType>*& resource_list_pointer) = 0;
		virtual IndexValueType GenerateNewListValue() = 0;
		virtual bool OnReleaseListFromMap(const IndexValueType& index,ILinearGrowListMember<IndexValueType>* resource_list_pointer) = 0;
	protected:
		ILinearGrowListMember<IndexValueType>* GetListData(const IndexValueType& list_index)
		{
			auto list_index_with_size = index_size_map.find(list_index);
			if (list_index_with_size == index_size_map.end())
			{
				LogError("Core", "could not find require index from list");
				return nullptr;
			}
			return value_data_map[list_index_with_size->second];
		};
	};
}
