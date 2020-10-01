#include"PancyMemoryBasic.h"
//GPU可访问的资源块
using namespace LunarEngine;
ResourceBlockGpu::ResourceBlockGpu(
	const uint64_t& memory_size_in,
	ComPtr<ID3D12Resource> resource_data_in,
	const D3D12_HEAP_TYPE& resource_usage_in,
	const D3D12_RESOURCE_STATES& resource_state
)
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
		now_res_load_state = RESOURCE_LOAD_CPU_FINISH;
	}
	else
	{
		map_pointer = NULL;
		now_res_load_state = RESOURCE_LOAD_GPU_FINISH;
	}
	now_subresource_state = resource_state;
}
PancyResourceLoadState ResourceBlockGpu::GetResourceLoadingState()
{
	if (if_start_copying_gpu && now_res_load_state == PancyResourceLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		bool if_GPU_finished = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->CheckGpuBrokenFence(wait_fence);
		if (if_GPU_finished)
		{
			//资源加载完毕
			now_res_load_state = PancyResourceLoadState::RESOURCE_LOAD_GPU_FINISH;
			if_start_copying_gpu = false;
		}
	}
	return now_res_load_state;
}
LResult ResourceBlockGpu::WaitForResourceLoading()
{
	//先检查资源是否正在往GPU中拷贝
	if (if_start_copying_gpu && now_res_load_state == PancyResourceLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		//等待资源拷贝结束
		auto check_error = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->WaitGpuBrokenFence(wait_fence);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//修改资源状态
		now_res_load_state = PancyResourceLoadState::RESOURCE_LOAD_GPU_FINISH;
		if_start_copying_gpu = false;
	}
	return g_Succeed;
}
LResult ResourceBlockGpu::ResourceBarrier(
	PancyRenderCommandList* commandlist,
	const D3D12_RESOURCE_STATES& last_state,
	const D3D12_RESOURCE_STATES& now_state
)
{
	if (now_subresource_state != last_state)
	{
		if (now_subresource_state == now_state)
		{
			return g_Succeed;
		}
		else
		{
			LResult error_message;
			LunarDebugLogError(E_FAIL, "resource state dismatch, could not change resource using state", error_message);

			return error_message;
		}
	}
	commandlist->GetCommandList()->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			resource_data.Get(),
			last_state,
			now_state
		)
	);
	//修改资源的访问格式
	now_subresource_state = now_state;
	return g_Succeed;
}
LResult ResourceBlockGpu::CopyFromDynamicBufferToGpu(
	PancyRenderCommandList* commandlist,
	ResourceBlockGpu& dynamic_buffer,
	const LunarResourceSize& src_offset,
	const LunarResourceSize& dst_offset,
	const LunarResourceSize& data_size
)
{
	if (now_res_load_state == RESOURCE_LOAD_FAILED)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource load failed, could not copy data to memory", error_message);

		return error_message;
	}
	if (dynamic_buffer.GetResourceLoadingState() != RESOURCE_LOAD_CPU_FINISH)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource haven't load to cpu, could not copy data to GPU", error_message);

		return error_message;
	}
	if (now_res_load_state != RESOURCE_LOAD_GPU_FINISH)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not a GPU resource, could not copy data to GPU", error_message);

		return error_message;
	}
	auto src_size_check = src_offset + data_size;
	auto dst_size_check = dst_offset + data_size;
	if (src_size_check > dynamic_buffer.GetSize() || dst_size_check > memory_size)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource size overflow, could not copy resource from cpu to GPU", error_message);

		return error_message;
	}
	auto check_error = ResourceBarrier(commandlist, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	commandlist->GetCommandList()->CopyBufferRegion(
		resource_data.Get(),
		dst_offset,
		dynamic_buffer.GetResource(),
		src_offset,
		data_size);
	check_error = ResourceBarrier(commandlist, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	now_res_load_state = RESOURCE_LOAD_GPU_LOADING;
	return g_Succeed;
}
LResult ResourceBlockGpu::CopyFromDynamicBufferToGpu(
	PancyRenderCommandList* commandlist,
	ResourceBlockGpu& dynamic_buffer,
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
	const LunarObjectID& Layout_num
)
{
	if (now_res_load_state == RESOURCE_LOAD_FAILED)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource load failed, could not copy data to memory", error_message);

		return error_message;
	}
	if (dynamic_buffer.GetResourceLoadingState() != RESOURCE_LOAD_CPU_FINISH)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource haven't laod to cpu, could not copy data to GPU", error_message);

		return error_message;
	}
	if (now_res_load_state != RESOURCE_LOAD_GPU_FINISH)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not a GPU resource, could not copy data to GPU", error_message);

		return error_message;
	}
	auto check_error = ResourceBarrier(commandlist, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	for (UINT i = 0; i < Layout_num; ++i)
	{
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT real_layout;
		real_layout.Footprint = pLayouts[i].Footprint;
		real_layout.Offset = pLayouts[i].Offset;

		CD3DX12_TEXTURE_COPY_LOCATION Dst(resource_data.Get(), i + 0);
		CD3DX12_TEXTURE_COPY_LOCATION Src(dynamic_buffer.GetResource(), real_layout);
		commandlist->GetCommandList()->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
	}
	check_error = ResourceBarrier(commandlist, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	now_res_load_state = RESOURCE_LOAD_GPU_LOADING;
	return g_Succeed;
}
LResult ResourceBlockGpu::SetResourceCopyBrokenFence(const PancyFenceIdGPU& broken_fence_id)
{
	if (now_res_load_state != PancyResourceLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "haven't copy any data to cpu, could not set broken fence", error_message);

		return error_message;
	}
	wait_fence = broken_fence_id;
	if_start_copying_gpu = true;
	return g_Succeed;
}
LResult ResourceBlockGpu::WriteFromCpuToBuffer(
	const LunarResourceSize& pointer_offset,
	const void* copy_data,
	const LunarResourceSize& data_size
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
	if (now_res_load_state != RESOURCE_LOAD_CPU_FINISH)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not a CPU copy resource, could not copy data to CPU", error_message);

		return error_message;
	}
	memcpy(map_pointer + pointer_offset, copy_data, data_size);
	now_res_load_state = RESOURCE_LOAD_CPU_FINISH;
	return g_Succeed;
}
LResult ResourceBlockGpu::WriteFromCpuToBuffer(
	const LunarResourceSize& pointer_offset,
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
	if (now_res_load_state != RESOURCE_LOAD_CPU_FINISH)
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
	now_res_load_state = RESOURCE_LOAD_CPU_FINISH;
	return g_Succeed;
}
LResult ResourceBlockGpu::ReadFromBufferToCpu(
	const LunarResourceSize& pointer_offset,
	void* copy_data,
	const LunarResourceSize data_size
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
LResult ResourceBlockGpu::BuildCommonDescriptorView(
	const BasicDescriptorDesc& descriptor_desc,
	const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
	bool if_wait_for_gpu
)
{
	if (if_wait_for_gpu)
	{
		auto check_error = WaitForResourceLoading();
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	PancyResourceLoadState now_load_state = GetResourceLoadingState();
	if (now_load_state == PancyResourceLoadState::RESOURCE_LOAD_CPU_FINISH || now_load_state == PancyResourceLoadState::RESOURCE_LOAD_GPU_FINISH)
	{
		switch (descriptor_desc.basic_descriptor_type)
		{
		case DescriptorTypeConstantBufferView:
		{
			BuildConstantBufferView(descriptor_desc.offset, descriptor_desc.block_size, DestDescriptor);
			break;
		}
		case DescriptorTypeShaderResourceView:
		{
			BuildShaderResourceView(DestDescriptor, descriptor_desc.shader_resource_view_desc);
			break;
		}
		case DescriptorTypeUnorderedAccessView:
		{
			BuildUnorderedAccessView(DestDescriptor, descriptor_desc.unordered_access_view_desc);
			break;
		}
		case DescriptorTypeRenderTargetView:
		{
			BuildRenderTargetView(DestDescriptor, descriptor_desc.render_target_view_desc);
			break;
		}
		case DescriptorTypeDepthStencilView:
		{
			BuildDepthStencilView(DestDescriptor, descriptor_desc.depth_stencil_view_desc);
			break;
		}
		default:
		{
			LResult error_message;
			LunarDebugLogError(E_FAIL, "resourceview is not a descriptor view", error_message);

			return error_message;
			break;
		}
		}
	}
	else
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not build finished,could not build descriptor view", error_message);

		return error_message;
	}
	return g_Succeed;
}
LResult ResourceBlockGpu::BuildVertexBufferView(
	const LunarResourceSize& offset,
	const LunarResourceSize& buffer_size,
	UINT StrideInBytes,
	D3D12_VERTEX_BUFFER_VIEW& VBV_out,
	bool if_wait_for_gpu
)
{
	if (if_wait_for_gpu)
	{
		auto check_error = WaitForResourceLoading();
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	PancyResourceLoadState now_load_state = GetResourceLoadingState();
	if (now_load_state == PancyResourceLoadState::RESOURCE_LOAD_CPU_FINISH || now_load_state == PancyResourceLoadState::RESOURCE_LOAD_GPU_FINISH)
	{
		//创建描述符
		VBV_out.BufferLocation = resource_data->GetGPUVirtualAddress() + offset;
		VBV_out.StrideInBytes = StrideInBytes;
		VBV_out.SizeInBytes = static_cast<UINT>(buffer_size);
	}
	else
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not a build finished,could not build descriptor view", error_message);

		return error_message;
	}
	return g_Succeed;
}
LResult ResourceBlockGpu::BuildIndexBufferView(
	const LunarResourceSize& offset,
	const LunarResourceSize& buffer_size,
	DXGI_FORMAT StrideInBytes,
	D3D12_INDEX_BUFFER_VIEW& IBV_out,
	bool if_wait_for_gpu
)
{
	if (if_wait_for_gpu)
	{
		auto check_error = WaitForResourceLoading();
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	PancyResourceLoadState now_load_state = GetResourceLoadingState();
	if (now_load_state == PancyResourceLoadState::RESOURCE_LOAD_CPU_FINISH || now_load_state == PancyResourceLoadState::RESOURCE_LOAD_GPU_FINISH)
	{
		//创建描述符
		IBV_out.BufferLocation = resource_data->GetGPUVirtualAddress() + offset;
		IBV_out.Format = StrideInBytes;
		IBV_out.SizeInBytes = static_cast<UINT>(buffer_size);
	}
	else
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "resource is not a build finished,could not build descriptor view", error_message);

		return error_message;
	}

	return g_Succeed;
}
void ResourceBlockGpu::BuildConstantBufferView(
	const LunarResourceSize& offset,
	const LunarResourceSize& block_size,
	const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor
)
{
	//根据资源数据创建描述符
	D3D12_CONSTANT_BUFFER_VIEW_DESC  CBV_desc;
	CBV_desc.BufferLocation = resource_data->GetGPUVirtualAddress() + offset;
	CBV_desc.SizeInBytes = static_cast<UINT>(block_size);
	//创建描述符
	PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateConstantBufferView(&CBV_desc, DestDescriptor);
}
void ResourceBlockGpu::BuildShaderResourceView(
	const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& SRV_desc
)
{
	//创建描述符
	PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateShaderResourceView(resource_data.Get(), &SRV_desc, DestDescriptor);
}
void ResourceBlockGpu::BuildRenderTargetView(
	const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
	const D3D12_RENDER_TARGET_VIEW_DESC& RTV_desc
)
{
	//创建描述符
	PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateRenderTargetView(resource_data.Get(), &RTV_desc, DestDescriptor);
}
void ResourceBlockGpu::BuildUnorderedAccessView(
	const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
	const D3D12_UNORDERED_ACCESS_VIEW_DESC& UAV_desc
)
{
	//创建描述符
	PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateUnorderedAccessView(
		resource_data.Get(),
		NULL,
		&UAV_desc,
		DestDescriptor
	);
}
void ResourceBlockGpu::BuildDepthStencilView(
	const D3D12_CPU_DESCRIPTOR_HANDLE& DestDescriptor,
	const D3D12_DEPTH_STENCIL_VIEW_DESC& DSV_desc
)
{
	//创建描述符
	PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateDepthStencilView(resource_data.Get(), &DSV_desc, DestDescriptor);
}
ResourceBlockGpu::~ResourceBlockGpu()
{
	if (resource_usage == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		resource_data->Unmap(0, NULL);
	}
}