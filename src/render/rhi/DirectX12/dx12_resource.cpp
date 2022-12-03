#include "render/rhi/DirectX12/dx12_resource.h"
#include "render/rhi/DirectX12/dx12_memory.h"
#include "render/rhi/DirectX12/dx12_swapchain.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/render_module.h"

using Microsoft::WRL::ComPtr;


namespace luna::render
{
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
