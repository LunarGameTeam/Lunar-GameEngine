#include "LDirectX12SwapChain.h"
using namespace luna;
using namespace Microsoft::WRL;
LDx12GraphicSwapChain::LDx12GraphicSwapChain(ID3D12CommandQueue* command_queue, const void* trarget_window, const LWindowRenderDesc& trarget_window_desc) :ILunarGraphicRenderSwapChain(trarget_window,trarget_window_desc)
{
	m_command_queue = command_queue;
	const HWND* window_hwnd_pointer = reinterpret_cast<const HWND*>(trarget_window);
	if (window_hwnd_pointer == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Create Directx swapchain Error ,empty hwnd", error_message);
	}
	m_window_hwnd = *window_hwnd_pointer;
}
void LDx12GraphicSwapChain::PresentFrame()
{
	directx_swap_chain->Present(1, 0);
}
uint32_t LDx12GraphicSwapChain::GetNowFrameID()
{
	return directx_swap_chain->GetCurrentBackBufferIndex();
}
LResult LDx12GraphicSwapChain::InitGraphicSwapChain()
{
	LResult check_error;
	check_error = ResetSwapChainData(m_render_window_desc);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult LDx12GraphicSwapChain::ResetSwapChainData(const LWindowRenderDesc& window_width_in)
{
	LResult check_error;
	//获取dxgi
	LDirectXGIFactory* directx_graphic_interface = reinterpret_cast<LDirectXGIFactory*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualGraphicInterface());
	if (directx_graphic_interface == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx graphic interface is broken ", error_message);
		return error_message;
	}
	//重新创建交换链
	if (directx_swap_chain != NULL)
	{
		directx_swap_chain.Reset();
	}
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = window_width_in.max_frame_number;
	swapChainDesc.Width = window_width_in.window_height;
	swapChainDesc.Height = window_width_in.window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	ComPtr<IDXGISwapChain1> swapChain;
	HRESULT hr = directx_graphic_interface->CreateSwapChainForHwnd(
		m_command_queue,
		m_window_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(0, "reset Swap Chain Error  ", error_message);
		return error_message;
	}
	swapChain.As(&directx_swap_chain);
	return g_Succeed;
}