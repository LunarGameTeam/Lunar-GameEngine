#include "DX12RenderQueue.h"
#include "DX12Swapchain.h"
#include "DX12Fence.h"
#include "Graphics/RenderModule.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"

using Microsoft::WRL::ComPtr;
using namespace luna;


namespace luna::render
{

DX12RenderQueue::DX12RenderQueue(RHIQueueType type)
{
	HRESULT hr;
	//创建直接渲染队列
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	switch (type)
	{
	case luna::render::RHIQueueType::eGraphic:
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	case luna::render::RHIQueueType::eTransfer:
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	default:
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	}
	ID3D12Device* directx_device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
	hr = directx_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mDirectQueue));
	assert(SUCCEEDED(hr));
	mDirectQueue->SetName(StringToWstring("direct queue").c_str());
}

DX12RenderQueue::~DX12RenderQueue()
{
}

void DX12RenderQueue::ExecuteCommandLists(RHIGraphicCmdList* cmdList)
{
	ID3D12CommandList* d12CmdList = cmdList->As<DX12GraphicCmdList>()->mDxCmdList.Get();
	mDirectQueue->ExecuteCommandLists(1, &d12CmdList);
};

ID3D12CommandQueue* DX12RenderQueue::GetCommandQueueByPipelineType(const RHICmdListType& pipeline_type)
{

	return mDirectQueue.Get();
}

RHISwapChainPtr DX12RenderQueue::CreateSwapChain(LWindow* window,const RHISwapchainDesc& windowDesc)
{	
	RHISwapChainPtr swapchain = CreateRHIObject<DX12SwapChain>(window, windowDesc);
	swapchain->As<DX12SwapChain>()->InitSwapchain(mDirectQueue.Get());
	return swapchain;
}

void DX12RenderQueue::Signal(RHIFence* fence, size_t fence_value)
{
	DX12Fence* dx_fence = fence->As<DX12Fence>();
	mDirectQueue->Signal(dx_fence->GetFenceObject(), fence_value);
}

void DX12RenderQueue::Wait(RHIFence* fence, uint64_t value)
{
	mDirectQueue->Wait(fence->As<DX12Fence>()->mFence.Get(), value);
}

}
