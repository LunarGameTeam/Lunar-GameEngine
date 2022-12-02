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

bool DX12SwapChain::Reset(const RHIWindowDesc& window_width_in)
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
		RHIResourcePtr back_rt_tex = CreateRHIObject<DX12ResourceNew>();
		DX12ResourceNew* dx12Res = back_rt_tex->As<DX12ResourceNew>();
		mSwapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&dx12Res->mDxRes));
		dx12Res->mDxDesc.MipLevels = 1;
		dx12Res->mDxDesc.DepthOrArraySize = 1;
		dx12Res->mResDesc.Desc.Width = window_width_in.mWidth;
		dx12Res->mResDesc.Desc.Height = window_width_in.mHeight;
		dx12Res->mResDesc.Desc.DepthOrArraySize = 1;		
		dx12Res->mResDesc.Desc.MipLevels = 1;
		
		back_rt_tex->mDimension = RHIResDimension::Texture2D;
		back_rt_tex->mFormat = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
		back_rt_tex->mResType = ResourceType::kTexture;		
		back_rt_tex->SetInitialState(ResourceState::kPresent);
		mBackBuffers.push_back(back_rt_tex);
	}
	return true;
}

uint32_t DX12SwapChain::NextImage(RHIFence* fence, uint64_t signal_value)
{
	uint32_t frame_index = GetNowFrameID();
	sRenderModule->GetCmdQueueCore()->Signal(fence, signal_value);	
	return frame_index;
}

}
