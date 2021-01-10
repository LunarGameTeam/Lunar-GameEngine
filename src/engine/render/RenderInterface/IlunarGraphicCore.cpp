#include"ILunarGraphicCore.h"
using namespace luna;
luna::ILunarGraphicDeviceCore::ILunarGraphicDeviceCore()
{
};
luna::ILunarGraphicDeviceCore::~ILunarGraphicDeviceCore()
{
}
luna::LResult luna::ILunarGraphicDeviceCore::Create()
{
	LResult check_error;
	check_error = InitDeviceData();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}

luna::ILunarGraphicRenderQueueCore::ILunarGraphicRenderQueueCore()
{

}
luna::LResult luna::ILunarGraphicRenderQueueCore::Create()
{
	LResult check_error;
	check_error = InitRenderQueueData();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}

luna::ILunarGraphicRenderCommondAllocator::ILunarGraphicRenderCommondAllocator(const LunarGraphicPipeLineType& commond_allocator_type):m_commond_allocator_type(commond_allocator_type)
{
}
luna::LResult luna::ILunarGraphicRenderCommondAllocator::Create()
{
	LResult check_error;
	check_error = InitCommondAllocatorData();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return luna::g_Succeed;
}
luna::ILunarGraphicRenderCommondList::ILunarGraphicRenderCommondList(const LunarGraphicPipeLineType& commond_list_type) :m_commond_list_work_state(COMMOND_LIST_STATE_NULL), m_commond_list_type(commond_list_type)
{
}
luna::LResult luna::ILunarGraphicRenderCommondList::Create(LSharedObject* allocator, LSharedObject* pipeline)
{
	luna::LResult check_error;
	check_error = InitCommondListData(allocator, pipeline);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
};
luna::LResult luna::ILunarGraphicRenderCommondList::ResetCommondList(LSharedObject* allocator, LSharedObject* pipeline)
{
	luna::LResult check_error;
	check_error = ResetCommondListData(allocator, pipeline);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}

luna::ILunarGraphicRenderFence::ILunarGraphicRenderFence():fence_value_self_add(1)
{
}
luna::LResult luna::ILunarGraphicRenderFence::Create()
{
	luna::LResult check_error;
	check_error = InitGraphicRenderFence();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}

luna::ILunarGraphicRenderSwapChain::ILunarGraphicRenderSwapChain(const void* trarget_window, const LWindowRenderDesc& trarget_window_desc) :m_render_window_desc(trarget_window_desc)
{
}
luna::LResult luna::ILunarGraphicRenderSwapChain::Create()
{
	luna::LResult check_error;
	check_error = InitGraphicSwapChain();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
luna::LResult luna::ILunarGraphicRenderSwapChain::ResetSwapChain(const LWindowRenderDesc& window_width_in)
{
	luna::LResult check_error;
	check_error = ResetSwapChainData(window_width_in);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}

luna::ILunarGraphicRenderDescriptor::ILunarGraphicRenderDescriptor(const LunarGraphicDescriptorType& descriptor_type, const size_t& m_descriptor_offset, const size_t &descriptor_size):
	m_descriptor_type(descriptor_type),
	m_descriptor_offset(m_descriptor_offset),
	m_descriptor_size(descriptor_size),
	m_descriptor_heap(this)
{
}
luna::LResult luna::ILunarGraphicRenderDescriptor::Create(LObject* descriptor_heap)
{
	luna::LResult check_error;
	check_error = InitGraphicRenderDescriptor(descriptor_heap);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	m_descriptor_heap = descriptor_heap;
	return g_Succeed;
}