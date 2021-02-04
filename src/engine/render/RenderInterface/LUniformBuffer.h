#pragma once
#include "core/core_module.h"
#include "core/file/path.h"
#include "core/misc/FastId.h"
#include "core/misc/misc.h"
#include "core/reflection/json_utility.h"
#include"core/reflection/reflection.h"
#include"core/Common/MultiThreadMap.h"
#include"core/asset/lasset.h"
#include"core/memory/ptr.h"
#include "core/Common/LinerGrowMap.h"
#include "ILunarGraphicCore.h"
#include "core/Math/lunar_math.h"
//uniform缓冲区
LInitNewFastIdClass(LUniformBufferID, size_t);
namespace luna
{
	struct LUniformBufferMemberDesc
	{
		LString variable_name;
		size_t buffer_size;
		size_t start_offset;
	};
	INIT_REFLECTION_CLASS(LUniformBufferMemberDesc,
		reflect_data.variable_name,
		reflect_data.buffer_size,
		reflect_data.start_offset
	);
	struct LUniformBufferDesc
	{
		size_t buffer_size;
		LVector<LUniformBufferMemberDesc> each_constant_member;
		bool operator== (const LUniformBufferDesc& p) const
		{
			bool if_same = true;
			if ((buffer_size != p.buffer_size) || (each_constant_member.size() != p.each_constant_member.size()))
			{
				if_same = false;
			}
			for (int32_t now_value_index = 0; now_value_index < each_constant_member.size(); ++now_value_index)
			{
				if (
					(each_constant_member[now_value_index].variable_name != p.each_constant_member[now_value_index].variable_name) ||
					(each_constant_member[now_value_index].buffer_size != p.each_constant_member[now_value_index].buffer_size) ||
					(each_constant_member[now_value_index].start_offset != p.each_constant_member[now_value_index].start_offset)
					)
				{
					if_same = false;
				}
			}
			return if_same;
		}
		const size_t GetHash() const
		{
			std::string hash_string = std::to_string(buffer_size);
			for (int32_t now_index_member = 0; now_index_member < each_constant_member.size(); ++now_index_member)
			{
				hash_string += "**";
				hash_string += each_constant_member[now_index_member].variable_name;
				hash_string += "**";
				hash_string += std::to_string(each_constant_member[now_index_member].buffer_size);
				hash_string += "**";
				hash_string += std::to_string(each_constant_member[now_index_member].start_offset);
				hash_string += "**";
			}
			return std::hash<std::string>()(hash_string);
		}
	};
	INIT_REFLECTION_CLASS(LUniformBufferDesc,
		reflect_data.buffer_size,
		reflect_data.each_constant_member
	);
	class LUniformBufferMemoryAllocator;
	class LUniformBufferMemoryBlock :public luna::LinerGrownResourceData<size_t>
	{
		size_t m_buffer_size;
		uint8_t* m_buffer_begin_pointer;
		LUnorderedMap<LString, LUniformBufferMemberDesc> m_member_variable;
	public:
		LUniformBufferMemoryBlock();
		~LUniformBufferMemoryBlock();
		LResult InitResource(const ValueMarkIndex<size_t>& data_index) override;
		LResult SetMatrix(const LString& variable, const luna::LunarMatrix4x4 &mat_data, const size_t& offset);
		LResult SetFloat4(const LString& variable, const luna::LunarVector4 &vector_data, const size_t& offset);
		LResult SetUint4(const  LString& variable, const luna::LunarUint4 &vector_data, const size_t& offset);
		LResult SetStruct(const LString& variable, const void* struct_data, const size_t& data_size, const size_t& offset);
		LResult SetFullValue(const void* data_pointer, const size_t& data_size);
	private:
		LResult ErrorVariableNotFind(const LString& variable_name);
	};

	class LUniformBufferMemoryList :public luna::LinerGrowListMemberOnlyOne<size_t>
	{
		LPtrBasic uniform_buffer_device_data;
	public:
		LUniformBufferMemoryList(
			const size_t& list_index,
			const size_t& max_list_size,
			LLinerGrowMap<size_t>* controler_map
		);
		~LUniformBufferMemoryList();
		uint8_t* GetBufferPointer(const size_t& value_index);
	private:
		LResult BuildNewValue(const ValueMarkIndex<size_t>& value_index) override;
		LResult ReleaseValue(const size_t& value_index) override;
		size_t ComputeRealBufferSizeByUniformSize(const size_t& uniform_size);
	};
	class LUniformBufferMemoryAllocator :public LLinerGrowMap<size_t>
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
		luna::LResult OnReleaseListFromMap(const size_t& index, ILinerGrowListMember<size_t>* resource_list_pointer) override;
	};
	void InitGraphicReflectValue()
	{
		InitNewStructToReflection(LUniformBufferMemberDesc);
		InitNewStructToReflection(LUniformBufferDesc);
	}
	class LUniformBufferAllocatorSystem 
	{
		LUnorderedMap<LUniformBufferDesc, LUniformBufferMemoryAllocator*> all_allocator_map;
	private:
		LUniformBufferAllocatorSystem();
	public:
		static LUniformBufferAllocatorSystem* CbufferAllocatorSystem;
		static LUniformBufferAllocatorSystem* GetInstance()
		{
			if (CbufferAllocatorSystem == nullptr)
			{
				CbufferAllocatorSystem = new LUniformBufferAllocatorSystem();
			}
			return CbufferAllocatorSystem;
		}
		LResult AllocateUinformBufferByDesc(const LUniformBufferDesc& uniform_buffer_desc, LUniformBufferMemoryBlock& allocate_data_from_map);
	};
}
namespace LContainerPack
{
	size_t hash_value(const luna::LUniformBufferDesc& key)
	{
		return key.GetHash();
	}
}