#include "LUniformBuffer.h"
using namespace luna;
luna::LUniformBufferMemoryBlock::LUniformBufferMemoryBlock() : luna::LinerGrownResourceData<size_t>()
{
};
luna::LUniformBufferMemoryBlock::~LUniformBufferMemoryBlock()
{

}
LResult luna::LUniformBufferMemoryBlock::InitResource(const ValueMarkIndex<size_t>& data_index)
{
	LUniformBufferMemoryAllocator* uniform_buffer_allocator = dynamic_cast<LUniformBufferMemoryAllocator*>(GetAllocator());
	if (uniform_buffer_allocator == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "uniform buffer alloctor is null", error_message);
	}
	auto uniform_buffer_desc = uniform_buffer_allocator->GetDesc();
	for (int32_t uniform_data_member_index = 0; uniform_data_member_index < uniform_buffer_desc.each_constant_member.size(); ++uniform_data_member_index)
	{
		m_member_variable.emplace(uniform_buffer_desc.each_constant_member[uniform_data_member_index].variable_name, uniform_buffer_desc.each_constant_member[uniform_data_member_index]);
	}
	m_buffer_size = uniform_buffer_desc.buffer_size;
	m_buffer_begin_pointer = uniform_buffer_allocator->GetBufferPointer(data_index);
	return luna::g_Succeed;
};
LResult luna::LUniformBufferMemoryBlock::SetMatrix(const LString& variable, const luna::LunarMatrix4x4& mat_data, const size_t& offset)
{
	auto start_pos = m_member_variable.find(variable);
	if (start_pos == m_member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	//传入Cbuffer中的矩阵需要做转置操作
	size_t matrix_size = sizeof(mat_data.GetMatrixValue());
	memcpy(m_buffer_begin_pointer + start_pos->second.start_offset + offset * matrix_size, &mat_data.TransposeMatrix().GetMatrixValue(), matrix_size);
	return g_Succeed;
}
LResult luna::LUniformBufferMemoryBlock::SetFloat4(const LString& variable, const luna::LunarVector4& vector_data, const size_t& offset)
{
	auto start_pos = m_member_variable.find(variable);
	if (start_pos == m_member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	size_t vec4_size = sizeof(vector_data.GetVectorValue());
	memcpy(m_buffer_begin_pointer + start_pos->second.start_offset + offset * vec4_size, &vector_data.GetVectorValue(), vec4_size);
	return g_Succeed;
}
LResult luna::LUniformBufferMemoryBlock::SetUint4(const  LString& variable, const luna::LunarUint4& vector_data, const size_t& offset)
{
	auto start_pos = m_member_variable.find(variable);
	if (start_pos == m_member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	size_t vec4_size = sizeof(vector_data.GetVectorValue());
	memcpy(m_buffer_begin_pointer + start_pos->second.start_offset + offset * vec4_size, &vector_data.GetVectorValue(), vec4_size);
	return g_Succeed;
}
LResult luna::LUniformBufferMemoryBlock::SetStruct(const LString& variable, const void* struct_data, const size_t& data_size, const size_t& offset)
{
	auto start_pos = m_member_variable.find(variable);
	if (start_pos == m_member_variable.end())
	{
		return ErrorVariableNotFind(variable);
	}
	memcpy(m_buffer_begin_pointer + start_pos->second.start_offset + offset * data_size, struct_data, data_size);
	return g_Succeed;
}
LResult luna::LUniformBufferMemoryBlock::SetFullValue(const void* data_pointer, const size_t& data_size)
{
	if (data_size != m_buffer_size)
	{
		LResult error_message;
		LunarDebugLogError(0, "Cbuffer Size Dismatch", error_message);
		return error_message;
	}
	memcpy(m_buffer_begin_pointer, data_pointer, data_size);
	return g_Succeed;
}
luna::LResult luna::LUniformBufferMemoryBlock::ErrorVariableNotFind(const LString& variable_name)
{
	LResult error_message;
	LUniformBufferMemoryAllocator* uniform_buffer_allocator = dynamic_cast<LUniformBufferMemoryAllocator*>(GetAllocator());
	if (uniform_buffer_allocator == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "uniform buffer alloctor is null", error_message);
	}
	LunarDebugLogError(0, "Could not find shader variable:" + variable_name + " in Cbuffer: " + uniform_buffer_allocator->GetName(), error_message);
	return error_message;
}



luna::LUniformBufferMemoryList::LUniformBufferMemoryList(
	const size_t& list_index,
	const size_t& max_list_size,
	LLinerGrowMap<size_t>* controler_map
) :luna::LinerGrowListMemberOnlyOne<size_t>(list_index, max_list_size, controler_map), uniform_buffer_device_data(lroot_object)
{
	LUniformBufferMemoryAllocator* uniform_buffer_allocator = dynamic_cast<LUniformBufferMemoryAllocator*>(GetAllocator());
	if (uniform_buffer_allocator == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "uniform buffer alloctor is null", error_message);
	}
	//根据每个uniformbuffer的大小，计算出每次扩展需要开辟的buffer大小
	size_t buffer_memory_size = ComputeRealBufferSizeByUniformSize(uniform_buffer_allocator->GetDesc().buffer_size);
	uniform_buffer_device_data = ILunarGraphicDeviceCore::GetInstance()->CreateUniforBuffer(buffer_memory_size);
	if (uniform_buffer_device_data.Get() == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "create uniform buffer failed", error_message);
	}

};
luna::LUniformBufferMemoryList::~LUniformBufferMemoryList()
{
};
uint8_t* luna::LUniformBufferMemoryList::GetBufferPointer(const size_t& value_index)
{
	uint8_t* basic_ptr = nullptr;
	basic_ptr = ILunarGraphicDeviceCore::GetInstance()->GetPointerFromSharedMemory(uniform_buffer_device_data.Get());
	if (basic_ptr == nullptr)
	{
		return nullptr;
	}
	size_t offset = 0;
	LUniformBufferMemoryAllocator* uniform_buffer_allocator = dynamic_cast<LUniformBufferMemoryAllocator*>(GetAllocator());
	if (uniform_buffer_allocator == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "uniform buffer alloctor is null", error_message);
	}
	offset = value_index * uniform_buffer_allocator->GetDesc().buffer_size;
	basic_ptr += offset;
	return basic_ptr;
}
LResult luna::LUniformBufferMemoryList::BuildNewValue(const ValueMarkIndex<size_t>& value_index)
{
	int a = 0;
	return luna::g_Succeed;
};
LResult luna::LUniformBufferMemoryList::ReleaseValue(const size_t& value_index)
{
	int a = 0;
	return luna::g_Succeed;
};
size_t luna::LUniformBufferMemoryList::ComputeRealBufferSizeByUniformSize(const size_t& uniform_size)
{
	if (uniform_size > CommonSize1K)
	{
		return CommonSize64K;
	}
	return CommonSize256K;
}



luna::LUniformBufferMemoryAllocator::LUniformBufferMemoryAllocator(const LUniformBufferDesc& cbuffer_desc, const size_t& max_size_per_list) :LLinerGrowMap<size_t>(max_size_per_list)
{
	m_cbuffer_desc = cbuffer_desc;
	//buffer的大小需要对齐256
	m_cbuffer_desc.buffer_size = luna::SizeAligned2Pow(m_cbuffer_desc.buffer_size, 256);
};
luna::LUniformBufferMemoryAllocator::~LUniformBufferMemoryAllocator()
{
};
const LString& luna::LUniformBufferMemoryAllocator::GetBufferResourceName()
{
	return m_buffer_resource_name;
}
LUniformBufferDesc luna::LUniformBufferMemoryAllocator::GetDesc()
{
	return m_cbuffer_desc;
};
const LString& luna::LUniformBufferMemoryAllocator::GetName()
{
	return m_buffer_resource_name;
};
uint8_t* luna::LUniformBufferMemoryAllocator::GetBufferPointer(const ValueMarkIndex<size_t>& data_index)
{
	auto list_data = GetListData(data_index.list_index);
	if (list_data == nullptr)
	{
		return nullptr;
	}
	LUniformBufferMemoryList* list_real_pointer = dynamic_cast<LUniformBufferMemoryList*>(list_data);
	if (list_real_pointer == nullptr)
	{
		luna::LResult error_message;
		LunarDebugLogError(0, "could not exchange memory list to LUniformBufferMemoryList", error_message);
		return nullptr;
	}
	return list_real_pointer->GetBufferPointer(data_index.data_size);
}
luna::LResult luna::LUniformBufferMemoryAllocator::BuildNewListToMap(const size_t& list_index, const size_t& max_size_per_list, ILinerGrowListMember<size_t>*& resource_list_pointer)
{
	resource_list_pointer = new LUniformBufferMemoryList(list_index, max_size_per_list, this);
	return luna::g_Succeed;
};
size_t luna::LUniformBufferMemoryAllocator::GenerateNewListValue()
{
	return uuid_generate.GetUuid().GetValue();
};
luna::LResult luna::LUniformBufferMemoryAllocator::OnReleaseListFromMap(const size_t& index, ILinerGrowListMember<size_t>* resource_list_pointer)
{
	return luna::g_Succeed;
};

LResult luna::LUniformBufferAllocatorSystem::AllocateUinformBufferByDesc(const LUniformBufferDesc& uniform_buffer_desc, LUniformBufferMemoryBlock& allocate_data_from_map)
{
	LResult check_error;;
	auto check_uniform_allocator = all_allocator_map.find(uniform_buffer_desc);
	if (check_uniform_allocator == all_allocator_map.end())
	{
		LunarDebugLogError(0, "could not find required uniform buffer alloctor", check_error);
		return check_error;
	}
	check_error = check_uniform_allocator->second->AllocatedDataFromMap(1, allocate_data_from_map);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}