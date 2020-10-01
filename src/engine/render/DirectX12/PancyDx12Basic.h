#pragma once
#include "core/core_module.h"
#include"core\reflection\PancyResourceJsonReflect.h"
#include"PancyTimeBasic.h"
//临时的dx11头文件(纹理压缩)
#include<d3d11.h>
#include<d3d12.h>
#include<DirectXMath.h>
//#include<d3d12shader.h>
#include<d3dcompiler.h>
#include"d3dx12.h"
#include<DXGI1_6.h>
#include <wrl/client.h>
#include<comdef.h>
#pragma comment ( lib, "D3D12.lib")
#pragma comment ( lib, "DXGI.lib")
#pragma comment ( lib, "DirectXTex.lib")
#pragma comment ( lib, "D3DCompiler.lib")
using Microsoft::WRL::ComPtr;

class PancyDx12DeviceBasic
{
	UINT FrameCount;
	//窗口信息
	HWND hwnd_window;
	uint32_t width;
	uint32_t height;
	//清理渲染队列的时候所使用的fence
	ComPtr<ID3D12Fence> queue_fence_direct;
	ComPtr<ID3D12Fence> queue_fence_compute;
	ComPtr<ID3D12Fence> queue_fence_copy;
	//交换链帧使用信息
	LunarObjectID current_frame_use;//当前帧的帧号
	LunarObjectID last_frame_use;//上一帧的帧号
	//dxgi设备(用于更新交换链)
	ComPtr<IDXGIFactory4> dxgi_factory;
	//d3d设备
	ComPtr<ID3D12Device> m_device;
	//交换链
	ComPtr<IDXGISwapChain3> dx12_swapchain;
	//渲染队列(direct类型,copy类型，compute类型)
	ComPtr<ID3D12CommandQueue> command_queue_direct;
	ComPtr<ID3D12CommandQueue> command_queue_copy;
	ComPtr<ID3D12CommandQueue> command_queue_compute;
	//默认的渲染目标
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
private:
	PancyDx12DeviceBasic(HWND hwnd_window, uint32_t width_in, uint32_t height_in);
public:
	static PancyDx12DeviceBasic* d3dbasic_instance;
	static LunarEngine::LResult SingleCreate(HWND hwnd_window_in, UINT wind_width_in, UINT wind_hight_in)
	{
		if (d3dbasic_instance != NULL)
		{
			return LunarEngine::g_Succeed;
		}
		else
		{
			d3dbasic_instance = new PancyDx12DeviceBasic(hwnd_window_in, wind_width_in, wind_hight_in);
			LunarEngine::LResult check_failed = d3dbasic_instance->Init();
			return check_failed;
		}
	}
	static PancyDx12DeviceBasic* GetInstance()
	{
		return d3dbasic_instance;
	}
	LunarEngine::LResult Init();
	LunarEngine::LResult ResetScreen(uint32_t window_width_in, uint32_t window_height_in);
	//获取com资源信息
	inline ID3D12Device* GetD3dDevice()
	{
		return m_device.Get();
	};
	inline ID3D12CommandQueue* GetCommandQueueDirect()
	{
		return command_queue_direct.Get();
	}
	inline ID3D12CommandQueue* GetCommandQueueCopy()
	{
		return command_queue_copy.Get();
	}
	inline ID3D12CommandQueue* GetCommandQueueCompute()
	{
		return command_queue_compute.Get();
	}
	inline LunarEngine::LResult SwapChainPresent(
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags)
	{
		HRESULT hr;
		hr = dx12_swapchain->Present(SyncInterval, Flags);
		if (FAILED(hr))
		{
			LunarEngine::LResult error_message;
			LunarDebugLogError(E_FAIL, "swapchain present error", error_message);
			return error_message;
		}
		last_frame_use = current_frame_use;
		current_frame_use = dx12_swapchain->GetCurrentBackBufferIndex();
		return LunarEngine::g_Succeed;
	}
	inline ID3D12Resource* GetBackBuffer(CD3DX12_CPU_DESCRIPTOR_HANDLE& heap_handle)
	{
		auto now_frame_use = dx12_swapchain->GetCurrentBackBufferIndex();
		auto rtv_offset = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), now_frame_use, rtv_offset);
		heap_handle = rtvHandle;
		return m_renderTargets[now_frame_use].Get();
	}

	//获取帧数字
	inline UINT GetFrameNum()
	{
		return FrameCount;
	}
	inline UINT GetNowFrame()
	{
		return current_frame_use;
	}
	inline UINT GetLastFrame()
	{
		return last_frame_use;
	}
	void FlushGpu();
private:
	void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
};



