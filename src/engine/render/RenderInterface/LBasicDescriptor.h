#pragma once
#pragma once
#include"ILunarGraphicCore.h"
//shader输入布局
namespace luna
{
	class LBasicDescriptor
	{
		luna::LString m_file_name;
		LPtrBasic m_descriptor_data;
	public:
		LBasicDescriptor(const luna::LString& file_name, LSharedObject* parent);
		~LBasicDescriptor()
		{
		};
		luna::LResult Create();
	};
	//页式描述符分配器
	class LRenderDescriptrMemoryPageAllocator :public LLinerGrowMap<size_t>
	{
		LString m_buffer_resource_name;
		LUniformBufferDesc m_cbuffer_desc;
		luna::LSafeIndexType::LUniformBufferIDIndexGennerator uuid_generate;
	public:
		LUniformBufferMemoryAllocator(const LUniformBufferDesc& cbuffer_desc, const size_t& max_size_per_list);
		~LUniformBufferMemoryAllocator();
		const LString& GetBufferResourceName();
		LUniformBufferDesc GetDesc();
		const LString& GetName();
		uint8_t* GetBufferPointer(const ValueMarkIndex<size_t>& data_index);
	private:
		luna::LResult BuildNewListToMap(const size_t& list_index, const size_t& max_size_per_list, ILinerGrowListMember<size_t>*& resource_list_pointer)override;
		size_t GenerateNewListValue() override;
		luna::LResult OnReleaseListFromMap(ILinerGrowListMember<size_t>* resource_list_pointer) override;
	};
}