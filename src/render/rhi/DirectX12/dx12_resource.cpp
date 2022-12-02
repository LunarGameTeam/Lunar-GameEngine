#include "render/rhi/DirectX12/dx12_resource.h"
#include "render/rhi/DirectX12/dx12_memory.h"
#include "render/rhi/DirectX12/dx12_swapchain.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/render_module.h"

using Microsoft::WRL::ComPtr;


namespace luna::render
{
LDirectx12ResourceBlock::LDirectx12ResourceBlock(
	const uint64_t& memory_size_in,
	ComPtr<ID3D12Resource> resource_data_in,
	const D3D12_HEAP_TYPE& resource_usage_in,
	const D3D12_RESOURCE_STATES& resource_state
)
{
	mIfStartCopyingGpu = false;
	memory_size = memory_size_in;
	mResData = resource_data_in;
	mHeapType = resource_usage_in;
	mResLoadState = RESOURCE_LOAD_FAILED;
	if (mHeapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		CD3DX12_RANGE readRange(0, 0);
		HRESULT hr = mResData->Map(0, &readRange, reinterpret_cast<void**>(&mMapPtr));
		if (FAILED(hr))
		{
			
			LogError("Device", "map dynamic buffer to cpu error");
		}
		mResLoadState = RESOURCE_LOAD_FINISH;
	}
	else
	{
		mMapPtr = NULL;
		mResLoadState = RESOURCE_LOAD_FINISH;
	}
	mNowResState = resource_state;
}

LDirectx12ResourceBlock::~LDirectx12ResourceBlock()
{
	if (mHeapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		mResData->Unmap(0, NULL);
	}
}

LDxResourceBlockLoadState LDirectx12ResourceBlock::GetResourceLoadingState()
{
	if (mIfStartCopyingGpu && mResLoadState == LDxResourceBlockLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		//bool if_GPU_finished = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->CheckGpuBrokenFence(wait_fence);
		//if (if_GPU_finished)
		{
			//资源加载完毕
			mResLoadState = LDxResourceBlockLoadState::RESOURCE_LOAD_FINISH;
			mIfStartCopyingGpu = false;
		}
	}
	return mResLoadState;
}

bool LDirectx12ResourceBlock::WaitForResourceLoading()
{
	//先检查资源是否正在往GPU中拷贝
	if (mIfStartCopyingGpu && mResLoadState == LDxResourceBlockLoadState::RESOURCE_LOAD_GPU_LOADING)
	{
		//等待资源拷贝结束
		//auto check_error = ThreadPoolGPUControl::GetInstance()->GetResourceLoadContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE_COPY)->WaitGpuBrokenFence(wait_fence);
		//if (!check_error)
		//{
		//	return check_error;
		//}
		//修改资源状态
		mResLoadState = LDxResourceBlockLoadState::RESOURCE_LOAD_FINISH;
		mIfStartCopyingGpu = false;
	}
	return true;
}

void LDirectx12ResourceBlock::WriteFromCpuToBuffer(
	const size_t& pointer_offset,
	const void* copy_data,
	const size_t& data_size
)
{
	assert(mResLoadState != RESOURCE_LOAD_FAILED);
	assert(mHeapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD);
	assert(mResLoadState == RESOURCE_LOAD_FINISH);
	memcpy(mMapPtr + pointer_offset, copy_data, data_size);
	mResLoadState = RESOURCE_LOAD_FINISH;
}

void LDirectx12ResourceBlock::WriteFromCpuToBuffer(
	const size_t& pointer_offset,
	LVector<D3D12_SUBRESOURCE_DATA>& subresources,
	LVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& pLayouts,
	LVector<UINT64>& pRowSizesInBytes,
	LVector<UINT>& pNumRows
)
{
	if (mResLoadState == RESOURCE_LOAD_FAILED)
	{
		
		LogError("Device", "resource load failed, could not copy data to memory");
		return;
	}
	if (mHeapType != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		
		LogError("Device", "resource type is not upload, could not copy data to memory");

		return;
	}
	if (mResLoadState != RESOURCE_LOAD_FINISH)
	{
		
		LogError("Device", "resource is not a CPU copy resource, could not copy data to CPU");
		return;
	}
	//获取待拷贝指针
	UINT8* pData = mMapPtr + pointer_offset;
	//获取subresource
	for (UINT i = 0; i < subresources.size(); ++i)
	{
		D3D12_MEMCPY_DEST DestData = {
			pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, pLayouts[i].Footprint.RowPitch * pNumRows[i]
		};
		MemcpySubresource(&DestData, &subresources[i], (SIZE_T)pRowSizesInBytes[i], pNumRows[i],
		                  pLayouts[i].Footprint.Depth);
	}
	mResLoadState = RESOURCE_LOAD_FINISH;
}

bool LDirectx12ResourceBlock::ReadFromBufferToCpu(
	const size_t& pointer_offset,
	void* copy_data,
	const size_t data_size
)
{
	if (mHeapType != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK)
	{
		
		LogError("Device", "resource type is not readback, could not read data back");

		return false;
	}
	return true;
	//todo: 回读GPU数据
}

//bool LDirectx12ResourceBlock::SetResourceCopyBrokenFence(const luna::LSafeIndexType::LGraphicFenceId& broken_fence_id)
//{
//	if (now_res_load_state != LDxResourceBlockLoadState::RESOURCE_LOAD_GPU_LOADING)
//	{
//		
//		LunarDebugLogError(E_FAIL, "haven't copy any data to cpu, could not set broken fence", error_message);
//
//		return false;
//	}
//	wait_fence = broken_fence_id;
//	if_start_copying_gpu = true;
//	return true;
//}
bool LDirectx12ResourceBlock::GetCpuMapPointer(UINT8** map_pointer_out)
{
	if (mResLoadState == RESOURCE_LOAD_FAILED)
	{
		LogError("Device", "resource load failed, could not copy data to memory");
		return false;
	}
	if (mHeapType != D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		LogError("Device", "resource type is not upload, could not copy data to memory");

		map_pointer_out = NULL;
		return false;
	}
	*map_pointer_out = mMapPtr;
	return true;
}


DX12Resource::DX12Resource(
	const RHIResDesc& resource_desc,
	const size_t& subresources_num,
	const D3D12_RESOURCE_STATES& resource_build_state
) : RHIResource(resource_desc), mSubResNum(subresources_num)
{
	InitResorceByDesc(resource_desc, resource_build_state);
}

DX12Resource::DX12Resource(
	const RHIMemory* res_heap,
	const size_t offset,
	const RHIResDesc& resource_desc,
	const size_t& subresources_num,
	const D3D12_RESOURCE_STATES& resource_build_state
) : RHIResource(resource_desc), mSubResNum(subresources_num)
{
	InitResorceByDesc(res_heap, offset, resource_desc, resource_build_state);
}
DX12Resource::DX12Resource(const RHIResDesc& resource_desc, const D3D12_RESOURCE_STATES& resource_build_state):
	RHIResource(resource_desc)
{
	InitResorceByDesc(resource_desc, resource_build_state);
}


DX12Resource::DX12Resource(
	const RHIMemory* res_heap,
	const size_t offset,
	const RHIResDesc& resource_desc,
	const D3D12_RESOURCE_STATES& resource_build_state
) : RHIResource(resource_desc)
{
	InitResorceByDesc(res_heap, offset, resource_desc, resource_build_state);
}

DX12Resource::DX12Resource(
	const int32_t backbufferIdx,
	RHISwapChain* swapchain) 
{

	DX12SwapChain* dx12Swapchain = swapchain->As<DX12SwapChain>();
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_data;	
	dx12Swapchain->mSwapChain->GetBuffer(backbufferIdx, IID_PPV_ARGS(&resource_data));	

	mResDesc.Desc = GetResourceDescDx(resource_data->GetDesc());
	mResDesc.ResHeapType = RHIHeapType::Default;
	mResBlock = new LDirectx12ResourceBlock(0, resource_data,
		D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
};

bool DX12Resource::_InitResorceByDesc(
	const RHIResDesc& buffer_desc,
	const D3D12_RESOURCE_STATES& resource_build_state,
	std::function<void(
		ID3D12Device* directx_device,
		_In_ const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES InitialResourceState,
		_In_opt_ const D3D12_CLEAR_VALUE* pOptimizedClearValue,
		REFIID riidResource,
		_COM_Outptr_opt_ void** ppvResource
	)> deal_func
)
{
	//获取directx设备
	ID3D12Device* directx_device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
	if (directx_device == nullptr)
	{
		
		LogError("Device", "Directx device is broken ");
		return false;
	}
	//在d3d层级上创建一个单独堆的buffer资源
	ComPtr<ID3D12Resource> resource_data;
	D3D12_HEAP_TYPE heap_type = GetHeapType(buffer_desc.ResHeapType);
	auto directx_res_desc = GetResourceDesc(buffer_desc.Desc);
	//初始化
	D3D12_CLEAR_VALUE default_clear;
	default_clear.Format = directx_res_desc.Format;
	D3D12_CLEAR_VALUE* clear_data = &default_clear;
	switch (buffer_desc.Desc.Format)
	{
	case RHITextureFormat::FORMAT_B8G8R8A8_UNORM:
	case RHITextureFormat::FORMAT_R8G8BB8A8_UNORM:
		default_clear.Color[0] = 0.0f;
		default_clear.Color[1] = 0.0f;
		default_clear.Color[2] = 0.0f;
		default_clear.Color[3] = 1.0f;
		if (directx_res_desc.Flags != D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
		{
			clear_data = nullptr;
		}
		break;
	case RHITextureFormat::FORMAT_D32_FLOAT:
	case RHITextureFormat::FORMAT_D24_UNORM_S8_UINT:
		default_clear.DepthStencil.Depth = 1.0f;
		default_clear.DepthStencil.Stencil = 0;
		if (directx_res_desc.Flags != D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
		{
			clear_data = nullptr;
		}
		break;
	default:
		clear_data = nullptr;
		break;
	}
	deal_func(
		directx_device,
		&directx_res_desc,
		resource_build_state,
		clear_data,
		IID_PPV_ARGS(&resource_data)
	);
	//计算缓冲区的大小，创建资源块
	directx_device->GetCopyableFootprints(&directx_res_desc, (UINT)mSubResNum, 1, 0, nullptr, nullptr,
	                                      nullptr, &mSubResSize);
	mResBlock = new
		LDirectx12ResourceBlock(mSubResSize, resource_data, heap_type, resource_build_state);
	//if (heap_type == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	//{
	//	check_error = m_resource_data->GetCpuMapPointer(&map_pointer);
	//	if (!check_error)
	//	{
	//		return check_error;
	//	}
	//}
	return true;
}

bool DX12Resource::InitResorceByDesc(
	const RHIResDesc& buffer_desc,
	const D3D12_RESOURCE_STATES& resource_build_state
)
{
	_InitResorceByDesc(
		buffer_desc,
		resource_build_state,
		[&](
		ID3D12Device* directx_device,
		_In_ const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES InitialResourceState,
		_In_opt_ const D3D12_CLEAR_VALUE* pOptimizedClearValue,
		REFIID riidResource,
		_COM_Outptr_opt_ void** ppvResource
	)
		{
			D3D12_HEAP_TYPE heap_type = GetHeapType(buffer_desc.ResHeapType);
			CD3DX12_HEAP_PROPERTIES new_desc = CD3DX12_HEAP_PROPERTIES(heap_type);
			auto directx_res_desc = GetResourceDesc(buffer_desc.Desc);
			HRESULT hr = directx_device->CreateCommittedResource(
				&new_desc,
				D3D12_HEAP_FLAG_NONE,
				&directx_res_desc,
				resource_build_state,
				pOptimizedClearValue,
				riidResource,
				ppvResource
			);
			if (FAILED(hr))
			{
				
				LogError("Device", "Create commit resource error");
			}
		}
	);
	return true;
}

void DX12Resource::WriteFromCpuToBuffer(
	const size_t& pointer_offset,
	const void* copy_data,
	const size_t& data_size
)
{
	mResBlock->WriteFromCpuToBuffer(pointer_offset, copy_data, data_size);
}

bool DX12Resource::InitResorceByDesc(
	const RHIMemory* res_heap,
	const size_t offset,
	const RHIResDesc& buffer_desc,
	const D3D12_RESOURCE_STATES& resource_build_state
)
{
	const DX12Memory* res_heap_pointer = dynamic_cast<const DX12Memory*>(res_heap);
	_InitResorceByDesc(
		buffer_desc,
		resource_build_state,
		[&](
		ID3D12Device* directx_device,
		_In_ const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES InitialResourceState,
		_In_opt_ const D3D12_CLEAR_VALUE* pOptimizedClearValue,
		REFIID riidResource,
		_COM_Outptr_opt_ void** ppvResource
	)
		{
			HRESULT hr = directx_device->CreatePlacedResource(
				res_heap_pointer->GetResourceHeap(),
				offset,
				pDesc,
				resource_build_state,
				pOptimizedClearValue,
				riidResource,
				ppvResource
			);
			if (FAILED(hr))
			{
				
				LogError("Device", "Create commit resource error");
			}
		}
	);
	return true;
}

void* DX12Resource::Map()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void DX12Resource::Unmap()
{
	throw std::logic_error("The method or operation is not implemented.");
}


void DX12ResourceNew::BindMemory(RHIMemory* memory, uint64_t offset)
{
	ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
	mBindMemory = memory;	

	// TODO
	if (mBindMemory->mMemoryDesc.Type == RHIHeapType::Upload)
		SetInitialState(ResourceState::kGenericRead);
	else
		SetInitialState(ResourceState::kCommon);

	DX12Memory* dx_memory = mBindMemory->As<DX12Memory>();
	device->CreatePlacedResource(
		dx_memory->mHeap.Get(),
		offset,
		&mDxDesc,
		DxConvertState(GetInitialState()),
		nullptr,
		IID_PPV_ARGS(&mDxRes));
	if (dx_memory->mHeap->GetDesc().Properties.Type == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		CD3DX12_RANGE readRange(0, 0);
		HRESULT hr = mDxRes->Map(0, &readRange, reinterpret_cast<void**>(&mMapPointer));
		if (FAILED(hr))
		{
			LogError("Device", "map dynamic buffer to cpu error");
		}
	}
	else
	{
		mMapPointer = NULL;
	}


}

void DX12ResourceNew::RefreshMemoryRequirements()
{
	ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
	D3D12_RESOURCE_ALLOCATION_INFO allocation_info = device->GetResourceAllocationInfo(0, 1, &mDxDesc);

	mMemoryLayout.size = allocation_info.SizeInBytes;
	mMemoryLayout.alignment = allocation_info.Alignment;
};

}
