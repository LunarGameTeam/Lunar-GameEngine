#pragma once
#include "render/render_config.h"
#include "dx12_rhi.h"
#include "dx12_resource.h"


namespace luna::render
{
using LDXGISwapchain = IDXGISwapChain3;


class RENDER_API DX12SwapChain : public RHISwapChain
{
public:
	ID3D12CommandQueue* mCmdQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
public:
	DX12SwapChain(LWindow* window, const RHISwapchainDesc& windowDesc) :
		RHISwapChain(window, windowDesc) {}

	void PresentFrame(RHIFence* fence, uint64_t waitValue) override;
	uint32_t GetNowFrameID() override;

	void GetD3D12Resource(const int32_t buffer_id, Microsoft::WRL::ComPtr<ID3D12Resource>& rt_texture)
	{
		mSwapChain->GetBuffer(buffer_id, IID_PPV_ARGS(&rt_texture));
	}

	uint32_t NextImage();

	bool InitSwapchain(ID3D12CommandQueue* cmdQueue);

private:
	
	bool Reset(const RHISwapchainDesc& desc) override;
};
}
