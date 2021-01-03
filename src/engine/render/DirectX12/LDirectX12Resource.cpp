#include "LDirectX12Resource.h"
using namespace luna;
using Microsoft::WRL::ComPtr;
LDirectx12ResourceBlock::LDirectx12ResourceBlock(
	const uint64_t& memory_size_in,
	ComPtr<ID3D12Resource> resource_data_in,
	const D3D12_HEAP_TYPE& resource_usage_in,
	const D3D12_RESOURCE_STATES& resource_state
) :wait_fence(0)
{
	if_start_copying_gpu = false;
	memory_size = memory_size_in;
	resource_data = resource_data_in;
	resource_usage = resource_usage_in;
	now_res_load_state = RESOURCE_LOAD_FAILED;
	if (resource_usage == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		CD3DX12_RANGE readRange(0, 0);
		HRESULT hr = resource_data->Map(0, &readRange, reinterpret_cast<void**>(&map_pointer));
		if (FAILED(hr))
		{
			LResult error_message;
			LunarDebugLogError(hr, "map dynamic buffer to cpu error", error_message);

		}
		now_res_load_state = RESOURCE_LOAD_FINISH;
	}
	else
	{
		map_pointer = NULL;
		now_res_load_state = RESOURCE_LOAD_FINISH;
	}
	now_subresource_state = resource_state;
}
LDirectx12ResourceBlock::~LDirectx12ResourceBlock()
{
	if (resource_usage == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		resource_data->Unmap(0, NULL);
	}
}
LDxResourceBlockLoadState LDirectx12ResourceBlock::GetResourceLoadingState()
{
	if (if_start_copying_gpu && now_res_load_state == LDxResourceBlockLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		//bool if_GPU_finished = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->CheckGpuBrokenFence(wait_fence);
		//if (if_GPU_finished)
		{
			//资源加载完毕
			now_res_load_state = LDxResourceBlockLoadState::RESOURCE_LOAD_FINISH;
			if_start_copying_gpu = false;
		}
	}
	return now_res_load_state;
}
LResult LDirectx12ResourceBlock::WaitForResourceLoading()
{
	//先检查资源是否正在往GPU中拷贝
	if (if_start_copying_gpu && now_res_load_state == LDxResourceBlockLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		//等待资源拷贝结束
		//auto check_error = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->WaitGpuBrokenFence(wait_fence);
		//if (!check_error.m_IsOK)
		//{
		//	return check_error;
		//}
		//修改资源状态
		now_res_load_state = LDxResourceBlockLoadState::RESOURCE_LOAD_FINISH;
		if_start_copying_gpu = false;
	}
	return g_Succeed;
}
LResult LDirectx12ResourceBlock::WriteFromCpuToBuffer(
	const size_t& pointer_offset,
	const void* copy_data,
	const size_t& data_size
)
{
	if (now_res_load_state == RESOURCE_LOAD_FAILED)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource load failed, could not copy data to memory", error_message);

		return error_message;
	}
	if (resource_usage != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource type is not upload, could not copy data to memory", error_message);

		return error_message;
	}
	if (now_res_load_state != RESOURCE_LOAD_FINISH)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not a CPU copy resource, could not copy data to CPU", error_message);

		return error_message;
	}
	memcpy(map_pointer + pointer_offset, copy_data, data_size);
	now_res_load_state = RESOURCE_LOAD_FINISH;
	return g_Succeed;
}
LResult LDirectx12ResourceBlock::WriteFromCpuToBuffer(
	const size_t& pointer_offset,
	std::vector<D3D12_SUBRESOURCE_DATA>& subresources,
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
	UINT64* pRowSizesInBytes,
	UINT* pNumRows
)
{
	if (now_res_load_state == RESOURCE_LOAD_FAILED)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource load failed, could not copy data to memory", error_message);
		return error_message;
	}
	if (resource_usage != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource type is not upload, could not copy data to memory", error_message);

		return error_message;
	}
	if (now_res_load_state != RESOURCE_LOAD_FINISH)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not a CPU copy resource, could not copy data to CPU", error_message);
		return error_message;
	}
	//获取待拷贝指针
	UINT8* pData = map_pointer + pointer_offset;
	//获取subresource
	D3D12_SUBRESOURCE_DATA* pSrcData = &subresources[0];
	UINT subres_size = static_cast<UINT>(subresources.size());
	for (UINT i = 0; i < subres_size; ++i)
	{
		D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, pLayouts[i].Footprint.RowPitch * pNumRows[i] };
		MemcpySubresource(&DestData, &pSrcData[i], (SIZE_T)pRowSizesInBytes[i], pNumRows[i], pLayouts[i].Footprint.Depth);
	}
	now_res_load_state = RESOURCE_LOAD_FINISH;
	return g_Succeed;
}
LResult LDirectx12ResourceBlock::ReadFromBufferToCpu(
	const size_t& pointer_offset,
	void* copy_data,
	const size_t data_size
)
{
	if (resource_usage != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource type is not readback, could not read data back", error_message);

		return error_message;
	}
	return g_Succeed;
	//todo: 回读GPU数据
}
LResult LDirectx12ResourceBlock::SetResourceCopyBrokenFence(const luna::LSafeIndexType::LGraphicFenceId& broken_fence_id)
{
	if (now_res_load_state != LDxResourceBlockLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "haven't copy any data to cpu, could not set broken fence", error_message);

		return error_message;
	}
	wait_fence = broken_fence_id;
	if_start_copying_gpu = true;
	return g_Succeed;
}
luna::LResult LDirectx12ResourceBlock::GetCpuMapPointer(UINT8** map_pointer_out)
{
	if (now_res_load_state == RESOURCE_LOAD_FAILED)
	{
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "resource load failed, could not copy data to memory", error_message);

		return error_message;
	}
	if (resource_usage != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		luna::LResult error_message;
		LunarDebugLogError(E_FAIL, "resource type is not upload, could not copy data to memory", error_message);

		map_pointer_out = NULL;
		return error_message;
	}
	*map_pointer_out = map_pointer;
	return luna::g_Succeed;
}