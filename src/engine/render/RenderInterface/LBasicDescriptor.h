#pragma once
#pragma once
#include"ILunarGraphicCore.h"
//shader输入布局
namespace luna
{
	class LBasicBindingObject
	{
	protected:
		LunarGraphicDescriptorType m_descriptor_type;
		size_t m_descriptor_offset;
		size_t m_descriptor_size;
		LPtr<ILunarGraphicRenderDescriptor> m_descriptor_data;
	public:
		LBasicBindingObject(
			const LunarGraphicDescriptorType &descriptor_type,
			LSharedObject* Parent):
			m_descriptor_type(descriptor_type),
			m_descriptor_offset(0),
			m_descriptor_size(0),
			m_descriptor_data(Parent)
		{
		}
		~LBasicBindingObject()
		{
		}
		LResult LBasicBindingObject::CreateByHeap(LObject* descriptor_heap_pointer,const size_t &descriptor_offset,const size_t& descriptor_size);
	};
	LResult LBasicBindingObject::CreateByHeap(LObject* descriptor_heap_pointer, const size_t &descriptor_offset, const size_t& descriptor_size)
	{
		m_descriptor_offset = descriptor_offset;
		m_descriptor_size = descriptor_size;
		if (descriptor_heap_pointer == nullptr)
		{
			LResult check_error;
			LunarDebugLogError(0, "descriptor heap of allocator is null", check_error);
			return check_error;
		}
		ILunarGraphicRenderDescriptor* descriptor_pointer = ILunarGraphicDeviceCore::GetInstance()->CreateDescriptor(m_descriptor_type, m_descriptor_offset, m_descriptor_size, descriptor_heap_pointer);
		if (descriptor_pointer == nullptr)
		{
			LResult check_error;
			LunarDebugLogError(0, "create descriptor fail", check_error);
			return check_error;
		}
		m_descriptor_data = descriptor_pointer;
		return g_Succeed;
	}
	class LBasicDescriptorBlock : public luna::LinerGrownResourceData<size_t>,public LSharedObject, public LBasicBindingObject
	{
	public:
		LBasicDescriptorBlock(const LunarGraphicDescriptorType& descriptor_type);
		~LBasicDescriptorBlock()
		{
		};
		LResult InitResource(const ValueMarkIndex<size_t>& data_index) override;
	};
	LBasicDescriptorBlock::LBasicDescriptorBlock(const LunarGraphicDescriptorType& descriptor_type): LBasicBindingObject(descriptor_type,this)
	{
		m_descriptor_type = descriptor_type;
	};
	LResult LBasicDescriptorBlock::InitResource(const ValueMarkIndex<size_t>& data_index)
	{
		LRenderDescriptrMemoryPageAllocator* descriptor_page_allocator = dynamic_cast<LRenderDescriptrMemoryPageAllocator*>(GetAllocator());
		auto real_descriptor_offset = descriptor_page_allocator->CountOffsetSizeByIndex(data_index);
		auto real_descriptor_size = data_index.data_size;
		LObject* descriptor_heap_pointer = dynamic_cast<LObject*>(descriptor_page_allocator->GetDescriptorHeap());
		if (descriptor_heap_pointer == nullptr)
		{
			LResult check_error;
			LunarDebugLogError(0, "descriptor heap of allocator is null", check_error);
			return check_error;
		}
		LResult check_error = CreateByHeap(descriptor_heap_pointer,real_descriptor_offset, real_descriptor_size);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return g_Succeed;
	};
	class LinerGrowListMemberDescriptor : public LinerGrowListMemberCommon<size_t>
	{
	public:
		LinerGrowListMemberDescriptor(
			const size_t& list_index,
			const size_t& max_list_size,
			LLinerGrowMap<size_t>* controler_map
		) :LinerGrowListMemberCommon<size_t>(list_index, max_list_size, controler_map)
		{
		};
		virtual ~LinerGrowListMemberDescriptor()
		{
		}
	private:
		LResult BuildNewValue(const ValueMarkIndex<size_t>& value_index) override
		{
			return luna::g_Succeed;
		};
		LResult ReleaseValue(const size_t& value_index) override
		{
			return luna::g_Succeed;
		};
	};
	//页式描述符分配器
	class LRenderDescriptrMemoryPageAllocator :public LLinerGrowMap<size_t>
	{
		LPtrBasic m_descriptor_heap;
		//当前分配器对应的描述符堆的起始位置
		size_t m_heap_start_offset;
		//当前分配器可分配的最大描述符数量
		size_t m_heap_control_size;
		//当前空闲的描述符页
		LUnorderedSet<size_t> m_empty_list_index;
	public:
		LRenderDescriptrMemoryPageAllocator(
			LBasicAsset* descriptor_heap,
			const size_t &heap_start_offset,
			const size_t& heap_control_size,
			const size_t& max_size_per_list
		);
		~LRenderDescriptrMemoryPageAllocator();
		LSharedObject* GetDescriptorHeap()
		{
			return m_descriptor_heap.Get();
		};
		size_t CountOffsetSizeByIndex(const ValueMarkIndex<size_t>& data_index)
		{
			size_t size_per_list = GetMaxSizePerList();
			size_t final_offset = m_heap_start_offset + size_per_list * data_index.list_index + data_index.data_index;
			if (final_offset + data_index.data_size >= m_heap_control_size)
			{
				LResult check_error;
				LunarDebugLogError(0, "size out of range", check_error);
				return -1;
			}
			return final_offset;
		}
	private:
		luna::LResult BuildNewListToMap(const size_t& list_index, const size_t& max_size_per_list, ILinerGrowListMember<size_t>*& resource_list_pointer)override;
		size_t GenerateNewListValue() override;
		luna::LResult OnReleaseListFromMap(const size_t& index, ILinerGrowListMember<size_t>* resource_list_pointer) override;
	};
	LRenderDescriptrMemoryPageAllocator::LRenderDescriptrMemoryPageAllocator(
		LBasicAsset* descriptor_heap,
		const size_t& heap_start_offset,
		const size_t& heap_control_size,
		const size_t& max_size_per_list
	) :
		m_descriptor_heap(lroot_object),
		m_heap_start_offset(heap_start_offset),
		m_heap_control_size(heap_control_size),
		LLinerGrowMap<size_t>(max_size_per_list)
	{
		//初始化所有的空闲页表
		size_t all_empty_page_size = heap_control_size / max_size_per_list;
		for (size_t index_empty_page = 0; index_empty_page < all_empty_page_size; ++index_empty_page)
		{
			m_empty_list_index.insert(index_empty_page);
		}
		m_descriptor_heap = descriptor_heap;
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
	//段式描述符分配器
	class LRenderDescriptrMemorySegmentalAllocator
	{
	};
}