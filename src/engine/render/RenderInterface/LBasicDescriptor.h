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
			const LunarGraphicDescriptorType& descriptor_type,
			LSharedObject* Parent);
		virtual ~LBasicBindingObject();
	protected:
		LResult LBasicBindingObject::CreateByHeap(LObject* descriptor_heap_pointer,const size_t &descriptor_offset,const size_t& descriptor_size);
	};
	
	class LBasicDescriptorBlockPage : public luna::LinerGrownResourceData<size_t>,public LSharedObject, public LBasicBindingObject
	{
	public:
		LBasicDescriptorBlockPage(const LunarGraphicDescriptorType& descriptor_type);
		~LBasicDescriptorBlockPage();
		LResult InitResource(const ValueMarkIndex<size_t>& data_index) override;
	};

	class LinerGrowListMemberDescriptor : public LinerGrowListMemberCommon<size_t>
	{
	public:
		LinerGrowListMemberDescriptor(
			const size_t& list_index,
			const size_t& max_list_size,
			LLinerGrowMap<size_t>* controler_map
		);
		virtual ~LinerGrowListMemberDescriptor();
	private:
		LResult BuildNewValue(const ValueMarkIndex<size_t>& value_index) override;
		LResult ReleaseValue(const size_t& value_index) override;
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
		LSharedObject* GetDescriptorHeap();
		size_t CountOffsetSizeByIndex(const ValueMarkIndex<size_t>& data_index);
	private:
		luna::LResult BuildNewListToMap(const size_t& list_index, const size_t& max_size_per_list, ILinerGrowListMember<size_t>*& resource_list_pointer)override;
		size_t GenerateNewListValue() override;
		luna::LResult OnReleaseListFromMap(const size_t& index, ILinerGrowListMember<size_t>* resource_list_pointer) override;
	};
	
	//段式描述符分配器
	class LRenderDescriptrMemorySegmentalAllocator;
	class LBasicDescriptorBlockSegmental : public SegmentalMemberValue, public LSharedObject, public LBasicBindingObject
	{
	public:
		LBasicDescriptorBlockSegmental(const LunarGraphicDescriptorType& descriptor_type);
		~LBasicDescriptorBlockSegmental();
		LResult OnSegmentalCreated(const size_t& data_offset, const size_t& data_size) override;
	};
	class LRenderDescriptrMemorySegmentalAllocator : public SegmentalAllocatorMap
	{
		LPtrBasic m_descriptor_heap;
		//当前分配器对应的描述符堆的起始位置
		size_t m_heap_start_offset;
	public:
		LRenderDescriptrMemorySegmentalAllocator(LBasicAsset* descriptor_heap,
			const size_t& heap_start_offset,
			const size_t& heap_control_size);
		~LRenderDescriptrMemorySegmentalAllocator();
		LSharedObject* GetDescriptorHeap();
		size_t CountOffsetSizeByAllocator(const size_t& data_offset);
	private:
		LResult BuildNewSegmentalToMap(const SegmentalIndex& required_segmental_index, SegmentalMemberValue* segmental_value) override;
		LResult ReleaseSegmentalFromMap(const SegmentalIndex& required_segmental_index) override;
	};
	//普通描述符分配器
	class CommonLunaDescriptorAllocator
	{
		LunarGraphicDescriptorType m_descriptor_type;
		LPtrBasic m_descriptor_heap;
		//当前分配器可分配的最大描述符数量
		size_t m_heap_control_size;
		float m_page_rate;
		size_t m_max_page_size;
		//页式描述符分配器(适用于分配较少刷新，且绑定数量小于m_max_page_size的绑定对象)
		LRenderDescriptrMemoryPageAllocator* descriptor_page_allocator;
		//段式描述符分配器(适用于分配默认每帧都会清空的，频繁更新且数量巨大的绑定池)
		LRenderDescriptrMemorySegmentalAllocator* descriptor_segmental_allocator;
	public:
		CommonLunaDescriptorAllocator(
			const LunarGraphicDescriptorType &descriptor_type,
			const size_t& heap_control_size,
			const float page_rate = 0.2f,
			const size_t max_page_size = 256
		);
		LResult CreateBindObjectSegmental(const size_t& alloc_require_size, LBasicDescriptorBlockSegmental*& bind_object_segmental);
		LResult CreateBindObjectPage(const size_t& alloc_require_size, LBasicDescriptorBlockPage*& bind_object_page);
		LResult Create();
	};
}