#include "Graphics/RHI/DirectX12/DX12Memory.h"

#include "Core/CoreMin.h"

#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RenderModule.h"

namespace luna::graphics
{
DX12Memory::DX12Memory(const RHIMemoryDesc& memoryDesc) : RHIMemory(memoryDesc)
{
	//获取directx设备
	ID3D12Device* dxDevice = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
	D3D12_HEAP_DESC desc{};
	desc.Alignment = memoryDesc.Alignment;
	desc.SizeInBytes = memoryDesc.SizeInBytes;
	switch (memoryDesc.Type)
	{
	default:
		break;
	case RHIHeapType::Default:
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		break;
	case RHIHeapType::Upload:
		desc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		break;
	case RHIHeapType::Readback:
		desc.Properties.Type = D3D12_HEAP_TYPE_READBACK;
		break;
	case RHIHeapType::Custom:
		break;
	}
	HRESULT hr = dxDevice->CreateHeap(&desc, IID_PPV_ARGS(&mHeap));
	assert(SUCCEEDED(hr));
}

}
