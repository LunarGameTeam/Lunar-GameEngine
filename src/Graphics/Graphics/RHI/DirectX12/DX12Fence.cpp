#include "Graphics/RHI/DirectX12/DX12Fence.h"
#include "Graphics/RHI/DirectX12/DX12RenderQueue.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"

namespace luna::graphics
{
DX12Fence::DX12Fence() : mEvent(nullptr)
{
	InitFence();
}

size_t DX12Fence::IncSignal(RHIRenderQueue* queue)
{
	DX12RenderQueue* d3d12Queue = dynamic_cast<DX12RenderQueue*>(queue);

	HRESULT hr = d3d12Queue->mDirectQueue->Signal(mFence.Get(), mValue);
	assert(SUCCEEDED(hr));	
	return mValue++;
}

uint64_t DX12Fence::GetCompletedValue()
{
	return  mFence->GetCompletedValue();
}

void DX12Fence::Wait(size_t fence_value_check)
{
	if (GetCompletedValue() < fence_value_check)
	{
		HRESULT hr;
		hr = mFence->SetEventOnCompletion(fence_value_check, mEvent);
		assert(SUCCEEDED(hr));		
		WaitForSingleObject(mEvent, INFINITE);
	}
}

bool DX12Fence::InitFence()
{	
	ID3D12Device* d3d12Device = sGlobelRenderDevice->As<DX12Device>()->GetDx12Device();
	assert(d3d12Device != nullptr);
	HRESULT hr = d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	assert(SUCCEEDED(hr));
	//创建一个同步fence消息的事件
	mEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(mEvent);
	return true;
}

}
