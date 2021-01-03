#pragma once
#include "LDirectXCommon.h"
namespace luna
{
	using LDXGISwapchain = IDXGISwapChain3;
	class LDx12GraphicSwapChain :public ILunarGraphicRenderSwapChain
	{
		ID3D12CommandQueue* m_command_queue;
		Microsoft::WRL::ComPtr<LDXGISwapchain> directx_swap_chain;
		HWND m_window_hwnd;
	public:
		LDx12GraphicSwapChain(ID3D12CommandQueue* command_queue,const void* trarget_window, const LWindowRenderDesc& trarget_window_desc);
		void PresentFrame() override;
		uint32_t GetNowFrameID() override;
	private:
		LResult InitGraphicSwapChain() override;
		LResult ResetSwapChainData(const LWindowRenderDesc& window_width_in) override;
	};
}                                                           