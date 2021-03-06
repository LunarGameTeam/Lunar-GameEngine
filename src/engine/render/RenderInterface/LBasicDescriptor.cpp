#include"LBasicDescriptor.h"
namespace luna
{
	LBasicBindingObject::LBasicBindingObject(
		const LunarGraphicDescriptorType& descriptor_type,
		LSharedObject* Parent) :
		m_descriptor_type(descriptor_type),
		m_descriptor_offset(0),
		m_descriptor_size(0),
		m_descriptor_data(Parent)
	{
	}
	LBasicBindingObject::~LBasicBindingObject()
	{
	}
	LResult LBasicBindingObject::CreateByHeap(LObject* descriptor_heap_pointer, const size_t& descriptor_offset, const size_t& descriptor_size)
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

	LBasicDescriptorBlockPage::LBasicDescriptorBlockPage(const LunarGraphicDescriptorType& descriptor_type) : LBasicBindingObject(descriptor_type, this)
	{
		m_descriptor_type = descriptor_type;
	};
	LBasicDescriptorBlockPage::~LBasicDescriptorBlockPage()
	{
	};
	LResult LBasicDescriptorBlockPage::InitResource(const ValueMarkIndex<size_t>& data_index)
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
		LResult check_error = CreateByHeap(descriptor_heap_pointer, real_descriptor_offset, real_descriptor_size);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return g_Succeed;
	};

	LinerGrowListMemberDescriptor::LinerGrowListMemberDescriptor(
		const size_t& list_index,
		const size_t& max_list_size,
		LLinerGrowMap<size_t>* controler_map
	) :LinerGrowListMemberCommon<size_t>(list_index, max_list_size, controler_map)
	{
	};
	LinerGrowListMemberDescriptor::~LinerGrowListMemberDescriptor()
	{
	}
	LResult LinerGrowListMemberDescriptor::BuildNewValue(const ValueMarkIndex<size_t>& value_index)
	{
		return luna::g_Succeed;
	};
	LResult LinerGrowListMemberDescriptor::ReleaseValue(const size_t& value_index)
	{
		return luna::g_Succeed;
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
		return g_Succeed;
	}
	LSharedObject* LRenderDescriptrMemoryPageAllocator::GetDescriptorHeap()
	{
		return m_descriptor_heap.Get();
	};
	size_t LRenderDescriptrMemoryPageAllocator::CountOffsetSizeByIndex(const ValueMarkIndex<size_t>& data_index)
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

	LBasicDescriptorBlockSegmental::LBasicDescriptorBlockSegmental(const LunarGraphicDescriptorType& descriptor_type) : LBasicBindingObject(descriptor_type, this)
	{
		m_descriptor_type = descriptor_type;
	};
	LBasicDescriptorBlockSegmental::~LBasicDescriptorBlockSegmental()
	{
	};
	LResult LBasicDescriptorBlockSegmental::OnSegmentalCreated(const size_t& data_offset, const size_t& data_size)
	{
		LRenderDescriptrMemorySegmentalAllocator* descriptor_segmental_allocator = dynamic_cast<LRenderDescriptrMemorySegmentalAllocator*>(GetAllocator());
		size_t real_descriptor_offset = descriptor_segmental_allocator->CountOffsetSizeByAllocator(data_offset);
		size_t real_descriptor_size = data_size;
		LObject* descriptor_heap_pointer = dynamic_cast<LObject*>(descriptor_segmental_allocator->GetDescriptorHeap());
		if (descriptor_heap_pointer == nullptr)
		{
			LResult check_error;
			LunarDebugLogError(0, "descriptor heap of allocator is null", check_error);
			return check_error;
		}
		LResult check_error = CreateByHeap(descriptor_heap_pointer, real_descriptor_offset, real_descriptor_size);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return g_Succeed;
	};


	LRenderDescriptrMemorySegmentalAllocator::LRenderDescriptrMemorySegmentalAllocator(LBasicAsset* descriptor_heap,
		const size_t& heap_start_offset,
		const size_t& heap_control_size) :
		m_descriptor_heap(lroot_object),
		m_heap_start_offset(heap_start_offset),
		SegmentalAllocatorMap(heap_control_size)
	{
		m_descriptor_heap = descriptor_heap;
	};
	LRenderDescriptrMemorySegmentalAllocator::~LRenderDescriptrMemorySegmentalAllocator()
	{
	}
	LSharedObject* LRenderDescriptrMemorySegmentalAllocator::GetDescriptorHeap()
	{
		return m_descriptor_heap.Get();
	};
	size_t LRenderDescriptrMemorySegmentalAllocator::CountOffsetSizeByAllocator(const size_t& data_offset)
	{
		size_t final_offset = m_heap_start_offset + data_offset;
		return final_offset;
	}
	LResult LRenderDescriptrMemorySegmentalAllocator::BuildNewSegmentalToMap(const SegmentalIndex& required_segmental_index, SegmentalMemberValue* segmental_value)
	{
		return g_Succeed;
	};
	LResult LRenderDescriptrMemorySegmentalAllocator::ReleaseSegmentalFromMap(const SegmentalIndex& required_segmental_index)
	{
		return g_Succeed;
	};

	CommonLunaDescriptorAllocator::CommonLunaDescriptorAllocator(
		const LunarGraphicDescriptorType& descriptor_type,
		const size_t& heap_control_size,
		const float page_rate,
		const size_t max_page_size
	) :
		m_descriptor_heap(lroot_object),
		m_descriptor_type(descriptor_type),
		m_heap_control_size(heap_control_size),
		m_page_rate(page_rate),
		m_max_page_size(max_page_size),
		descriptor_page_allocator(nullptr),
		descriptor_segmental_allocator(nullptr)
	{

	}
	LResult CommonLunaDescriptorAllocator::Create()
	{
		LResult check_error;
		LBasicAsset* descriptor_heap = ILunarGraphicDeviceCore::GetInstance()->CreateDescriptorHeapByType(m_descriptor_type, m_heap_control_size);
		if (descriptor_heap != nullptr)
		{
			m_descriptor_heap = descriptor_heap;
		}
		else
		{
			return g_Failed;
		}
		size_t page_end = static_cast<size_t>(static_cast<double>(m_heap_control_size) * static_cast<double>(m_page_rate));
		descriptor_page_allocator = new LRenderDescriptrMemoryPageAllocator(descriptor_heap, 0, page_end, m_max_page_size);
		descriptor_segmental_allocator = new LRenderDescriptrMemorySegmentalAllocator(descriptor_heap, page_end, m_heap_control_size - page_end);
		check_error = descriptor_segmental_allocator->Create();
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return g_Succeed;
	}
	LResult CommonLunaDescriptorAllocator::CreateBindObjectSegmental(const size_t& alloc_require_size, LBasicDescriptorBlockSegmental*& bind_object_segmental)
	{
		LResult check_error;
		if (descriptor_segmental_allocator == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "segmental allocator not created", error_message);
			return error_message;
		}
		if (bind_object_segmental != nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "segmental pointer not empty", error_message);
			return error_message;
		}
		bind_object_segmental = luna::LCreateObject<LBasicDescriptorBlockSegmental>(m_descriptor_type);
		if (bind_object_segmental == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "create segmental object fail", error_message);
			return error_message;
		}
		check_error = descriptor_segmental_allocator->AllocateNewSegmental(alloc_require_size, bind_object_segmental);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return g_Succeed;
	}
	LResult CommonLunaDescriptorAllocator::CreateBindObjectPage(const size_t& alloc_require_size, LBasicDescriptorBlockPage*& bind_object_page)
	{
		LResult check_error;
		if (descriptor_page_allocator == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "page allocator not created", error_message);
			return error_message;
		}
		if (bind_object_page != nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "page pointer not empty", error_message);
			return error_message;
		}
		bind_object_page = luna::LCreateObject<LBasicDescriptorBlockPage>(m_descriptor_type);
		if (bind_object_page == nullptr)
		{
			LResult error_message;
			LunarDebugLogError(0, "create page object fail", error_message);
			return error_message;
		}
		check_error = descriptor_page_allocator->AllocatedDataFromMap(alloc_require_size, bind_object_page);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		return g_Succeed;
	}
}