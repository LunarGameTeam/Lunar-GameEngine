#include "render/rhi/DirectX12/dx12_swapchain.h"
#include "render/rhi/DirectX12/dx12_render_queue.h"
#include "render/rhi/DirectX12/dx12_device.h"

#include "render/render_module.h"

using namespace Microsoft::WRL;


namespace luna::render
{

void DX12SwapChain::PresentFrame(RHIFence* fence, uint64_t waitValue)
{
	fence->Wait(waitValue);
	mSwapChain->Present(1, 0);
}

uint32_t DX12SwapChain::GetNowFrameID()
{
	return mSwapChain->GetCurrentBackBufferIndex();
}

bool DX12SwapChain::InitSwapchain(ID3D12CommandQueue* cmdQueue)
{
	mCmdQueue = cmdQueue;
	assert(Reset(mWindowDesc));
	return true;
}

bool DX12SwapChain::Reset(const RHISwapchainDesc& window_width_in)
{
	LDirectXGIFactory* dxGIFactor = sRenderModule->GetDevice<DX12Device>()->GetDxGIFactory();
	assert(dxGIFactor != nullptr);
	//重新创建交换链
	if (mSwapChain != NULL)
	{
		mSwapChain.Reset();
	}
	mBackBuffers.clear();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = window_width_in.mFrameNumber;
	swapChainDesc.Width = window_width_in.mWidth;
	swapChainDesc.Height = window_width_in.mHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	ComPtr<IDXGISwapChain1> swapChain;
	HRESULT hr = dxGIFactor->CreateSwapChainForHwnd(
		mCmdQueue,
		mWindow->GetWin32HWND(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);
	assert(SUCCEEDED(hr));
	swapChain.As(&mSwapChain);
	for (uint32_t frameIndex = 0; frameIndex < swapChainDesc.BufferCount; ++frameIndex)
	{
		RHIResourcePtr back_rt_tex = CreateRHIObject<DX12Resource>(frameIndex,this);
		mBackBuffers.push_back(back_rt_tex);

		render::ViewDesc backbufferViewDesc;

		backbufferViewDesc.mBaseMipLevel = 0;
		backbufferViewDesc.mViewType = render::RHIViewType::kRenderTarget;
		backbufferViewDesc.mViewDimension = render::RHIViewDimension::TextureView2D;
		RHIViewPtr back_rt_view = sRenderModule->GetRenderDevice()->CreateView(backbufferViewDesc);
		back_rt_view->BindResource(back_rt_tex);
		mViews.push_back(back_rt_view);
	}
	return true;
}

uint32_t DX12SwapChain::NextImage()
{
	uint32_t frame_index = GetNowFrameID();
	return frame_index;
}

}
