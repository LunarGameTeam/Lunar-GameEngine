#include"LDirectX12CommondList.h"
using namespace luna;
luna::LDx12GraphicRenderCommondList::LDx12GraphicRenderCommondList(const luna::LunarGraphicPipeLineType& commond_list_type):
	ILunarGraphicRenderCommondList(commond_list_type)
{
}
LResult luna::LDx12GraphicRenderCommondList::InitCommondListData(LSharedObject* allocator, LSharedObject* pipeline)
{
	LResult check_error;
	//获取directx设备
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
		return error_message;
	}
	//获取命令分配器
	LDx12GraphicRenderCommondAllocator* common_allocator_data = dynamic_cast<LDx12GraphicRenderCommondAllocator*>(allocator);
	if (common_allocator_data == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "commond allocator is broken ", error_message);
		return error_message;
	}
	//获取pipeline
	LDx12GraphicResourcePipelineState* pipeline_data = dynamic_cast<LDx12GraphicResourcePipelineState*>(pipeline);
	if (common_allocator_data == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "pipeline state is broken ", error_message);
		return error_message;
	}
	//检查格式是否匹配
	auto &now_commond_list_type = GetPipelineType();
	if ((now_commond_list_type != common_allocator_data->GetPipelineType()) || (now_commond_list_type != pipeline_data->GetDesc().pipeline_type))
	{
		LResult error_message;
		LunarDebugLogError(0, "CommandList type dismatch allocator/pipeline type", error_message);
		return error_message;
	}
	//获取list格式
	D3D12_COMMAND_LIST_TYPE commond_list_type_directx;
	check_error = GetDirectXCommondlistType(now_commond_list_type, commond_list_type_directx);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//创建commondlist
	HRESULT hr = directx_device->CreateCommandList(0, commond_list_type_directx, common_allocator_data->GetAllocator(), pipeline_data->GetPipeLine(), IID_PPV_ARGS(&directx_commond_list));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "Create CommandList Error When build commandlist", error_message);
		return error_message;
	}
	return g_Succeed;
};
LResult luna::LDx12GraphicRenderCommondList::ResetCommondListData(LSharedObject* allocator, LSharedObject* pipeline)
{
	//获取命令分配器
	LDx12GraphicRenderCommondAllocator* common_allocator_data = dynamic_cast<LDx12GraphicRenderCommondAllocator*>(allocator);
	if (common_allocator_data == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "commond allocator is broken ", error_message);
		return error_message;
	}
	//获取pipeline
	LDx12GraphicResourcePipelineState* pipeline_data = dynamic_cast<LDx12GraphicResourcePipelineState*>(pipeline);
	if (common_allocator_data == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "pipeline state is broken ", error_message);
		return error_message;
	}
	//检查格式是否匹配
	auto& now_commond_list_type = GetPipelineType();
	if ((now_commond_list_type != common_allocator_data->GetPipelineType()) || (now_commond_list_type != pipeline_data->GetDesc().pipeline_type))
	{
		LResult error_message;
		LunarDebugLogError(0, "CommandList type dismatch allocator/pipeline type", error_message);
		return error_message;
	}
	HRESULT hr = directx_commond_list->Reset(common_allocator_data->GetAllocator(), pipeline_data->GetPipeLine());
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "reset CommandList Error When build commandlist", error_message);
		return error_message;
	}
	return g_Succeed;
}

luna::LDx12GraphicRenderCommondAllocator::LDx12GraphicRenderCommondAllocator(const LunarGraphicPipeLineType& commond_allocator_type) : ILunarGraphicRenderCommondAllocator(commond_allocator_type)
{
}
LResult LDx12GraphicRenderCommondAllocator::InitCommondAllocatorData()
{
	LResult check_error;
	//获取directx设备
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
		return error_message;
	}
	HRESULT hr;
	//创建并解锁alloctor
	D3D12_COMMAND_LIST_TYPE commond_allocator_type_directx;
	check_error = GetDirectXCommondlistType(GetPipelineType(), commond_allocator_type_directx);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	hr = directx_device->CreateCommandAllocator(commond_allocator_type_directx, IID_PPV_ARGS(&directx_commond_allocator));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(0, "Create Direct CommandAllocator Error When init D3D basic ", error_message);
		return error_message;
	}
	return g_Succeed;
};