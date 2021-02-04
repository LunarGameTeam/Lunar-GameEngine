#pragma once
#pragma once
#include"ILunarGraphicCore.h"
//shader输入布局
namespace luna
{
	class LBasicDescriptorBlock : public luna::LinerGrownResourceData<size_t>,public LSharedObject
	{
		LunarGraphicDescriptorType m_descriptor_type;
		size_t m_descriptor_offset;
		size_t m_descriptor_size;
		LPtr<ILunarGraphicRenderDescriptor> m_descriptor_data;
	public:
		LBasicDescriptorBlock(const luna::LString& file_name);
		~LBasicDescriptorBlock()
		{
		};
		LResult InitResource(const ValueMarkIndex<size_t>& data_index) override;
	};
	LBasicDescriptorBlock::LBasicDescriptorBlock(const luna::LString& file_name) : m_descriptor_data(this)
	{
		LRenderDescriptrMemoryPageAllocator* descriptor_page_allocator = dynamic_cast<LRenderDescriptrMemoryPageAllocator*>(GetAllocator());
		LObject* descriptor_heap_pointer = descriptor_page_allocator->GetDescriptorHeap();
		ILunarGraphicDeviceCore::GetInstance()->CreateDescriptor(m_descriptor_type, m_descriptor_offset, m_descriptor_size, descriptor_heap_pointer);
	};
	LResult LBasicDescriptorBlock::InitResource(const ValueMarkIndex<size_t>& data_index)
	{
	};
	class LinerGrowListMemberDescriptor : public ILinerGrowListMember<size_t>
	{
		//已经被分配使用的数据的id
		LUnorderedSet<size_t> now_use_data;
		//尚未被分配使用的数据id
		LUnorderedSet<size_t> now_empty_data;
	public:
		LinerGrowListMemberDescriptor(
			const size_t& list_index,
			const size_t& max_list_size,
			LLinerGrowMap<size_t>* controler_map
		) :ILinerGrowListMember<size_t>(list_index, max_list_size, controler_map)
		{
			for (size_t index_now = 0; index_now < max_list_size; ++index_now)
			{
				now_empty_data.insert(index_now);
			}
		};
		virtual ~LinerGrowListMemberDescriptor()
		{
		}
		const size_t GetEmptySize() override
		{
			return now_empty_data.size();
		}
		LResult BuildNewValueFromList(const size_t& require_size, LinerGrownResourceData<size_t>& value_data) override
		{
			LResult check_error;
			if (value_data.CheckIfCreated())
			{
				LunarDebugLogError(0, "do not repeate create resource", check_error);
				return check_error;
			}
			if (require_size != static_cast<size_t>(1))
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
			ValueMarkIndex<size_t> now_value_data_mark;
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
		LResult ReleaseValueFromList(const size_t& value_index) override
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
		virtual LResult BuildNewValue(const ValueMarkIndex<size_t>& value_index) = 0;
		virtual LResult ReleaseValue(const size_t& value_index) = 0;
	};
	//页式描述符分配器
	class LRenderDescriptrMemoryPageAllocator :public LLinerGrowMap<size_t>
	{
		LPtrBasic m_descriptor_heap;
		//当前分配器对应的描述符堆的起始位置
		size_t m_heap_start_offset;
		//当前分配器可分配的最大描述符数量
		size_t m_heap_control_size;
		//当前描述符分配器为每个描述符页分配的描述符数量
		size_t m_descriptor_size_per_page;
		//当前空闲的描述符页
		LUnorderedSet<size_t> m_empty_list_index;
	public:
		LRenderDescriptrMemoryPageAllocator(
			LBasicAsset* descriptor_heap,
			const size_t &heap_start_offset,
			const size_t& heap_control_size,
			const size_t &descriptor_size_per_page,
			const size_t& max_size_per_list
		);
		~LRenderDescriptrMemoryPageAllocator();
		LSharedObject* GetDescriptorHeap()
		{
			return m_descriptor_heap.Get();
		};
	private:
		luna::LResult BuildNewListToMap(const size_t& list_index, const size_t& max_size_per_list, ILinerGrowListMember<size_t>*& resource_list_pointer)override;
		size_t GenerateNewListValue() override;
		luna::LResult OnReleaseListFromMap(const size_t& index, ILinerGrowListMember<size_t>* resource_list_pointer) override;
	};
	LRenderDescriptrMemoryPageAllocator::LRenderDescriptrMemoryPageAllocator(
		LBasicAsset* descriptor_heap,
		const size_t& heap_start_offset,
		const size_t& heap_control_size,
		const size_t& descriptor_size_per_page,
		const size_t& max_size_per_list
	) :
		m_descriptor_heap(lroot_object),
		m_heap_start_offset(heap_start_offset),
		m_heap_control_size(heap_control_size),
		m_descriptor_size_per_page(descriptor_size_per_page),
		LLinerGrowMap<size_t>(heap_control_size/ descriptor_size_per_page)
	{
		//初始化所有的空闲页表
		size_t all_empty_page_size = heap_control_size / descriptor_size_per_page;
		for (size_t index_empty_page = 0; index_empty_page < all_empty_page_size; ++index_empty_page)
		{
			m_empty_list_index.insert(index_empty_page);
		}
		m_descriptor_heap = ILunarGraphicDeviceCore::GetInstance()->CreateDescriptorHeapFromFile(,);
	}
	LRenderDescriptrMemoryPageAllocator::~LRenderDescriptrMemoryPageAllocator()
	{
	}
	size_t LRenderDescriptrMemoryPageAllocator::GenerateNewListValue()
	{
		if (!m_empty_list_index.empty())
		{
			size_t now_index = *m_empty_list_index.begin();
			m_empty_list_index.erase(now_index);
			return now_index;
		}
		else
		{
			luna::LResult error_message;
			LunarDebugLogError(0, "descriptor allocator is full", error_message);
			return size_t(-1);
		}
	};
	luna::LResult LRenderDescriptrMemoryPageAllocator::OnReleaseListFromMap(const size_t& index, ILinerGrowListMember<size_t>* resource_list_pointer)
	{
		m_empty_list_index.insert(index);
		return g_Succeed;
	}
	luna::LResult LRenderDescriptrMemoryPageAllocator::BuildNewListToMap(const size_t& list_index, const size_t& max_size_per_list, ILinerGrowListMember<size_t>*& resource_list_pointer)
	{

	}

}