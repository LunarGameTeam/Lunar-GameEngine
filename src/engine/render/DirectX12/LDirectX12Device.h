#pragma once
#include "LDirectXCommon.h"
namespace luna
{
	class ILunarDirectXGraphicDeviceCore :public ILunarGraphicDeviceCore
	{
		Microsoft::WRL::ComPtr<LDirectXGIFactory> m_dxgi_factory;
		Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	public:
		ILunarDirectXGraphicDeviceCore();
		~ILunarDirectXGraphicDeviceCore();
		uint8_t* GetPointerFromSharedMemory(LSharedObject* dynamic_buffer_pointer) override;
		AddNewGraphicDeviceFuncOverride(Shader);
		AddNewGraphicDeviceFuncOverride(RootSignature);
		AddNewGraphicDeviceFuncOverride(Pipeline);

		AddNewGraphicDeviceFuncOverride(BufferResource);
		AddNewGraphicDeviceFuncOverride(TextureResource);
		AddNewGraphicDeviceFuncOverride(ResourceHeap);

		AddNewGraphicDeviceFuncOverride(Descriptor);
		AddNewGraphicDeviceFuncOverride(DescriptorHeap);
		void* GetVirtualDevice() override
		{
			return m_device.Get();
		};
		void* GetVirtualGraphicInterface()
		{
			return m_dxgi_factory.Get();
		}
		LBasicAsset* CreateUniforBuffer(const size_t& uniform_buffer_size) override;
		ILunarGraphicRenderCommondList* CreateCommondList(
			LSharedObject* allocator,
			LSharedObject* pipeline,
			const LunarGraphicPipeLineType& pipeline_type
		) override;
		ILunarGraphicRenderCommondAllocator* CreateCommondAllocator(const LunarGraphicPipeLineType& pipeline_type) override;
		ILunarGraphicRenderFence* CreateFence()override;
	private:
		LResult InitDeviceData() override;
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		//全局反射信息注册
		void AddDirectXEnumVariable();
	};
}



//class PancyDx12DeviceBasic
//{
//	UINT FrameCount;
//	//窗口信息
//	HWND hwnd_window;
//	uint32_t width;
//	uint32_t height;
//	//清理渲染队列的时候所使用的fence
//	ComPtr<ID3D12Fence> queue_fence_direct;
//	ComPtr<ID3D12Fence> queue_fence_compute;
//	ComPtr<ID3D12Fence> queue_fence_copy;
//	//交换链帧使用信息
//	LunarObjectID current_frame_use;//当前帧的帧号
//	LunarObjectID last_frame_use;//上一帧的帧号
//	//dxgi设备(用于更新交换链)
//	ComPtr<IDXGIFactory4> dxgi_factory;
//	//d3d设备
//	ComPtr<ID3D12Device> m_device;
//	//交换链
//	ComPtr<IDXGISwapChain3> dx12_swapchain;
//	//渲染队列(direct类型,copy类型，compute类型)
//	ComPtr<ID3D12CommandQueue> command_queue_direct;
//	ComPtr<ID3D12CommandQueue> command_queue_copy;
//	ComPtr<ID3D12CommandQueue> command_queue_compute;
//	//默认的渲染目标
//	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
//	LVector<ComPtr<ID3D12Resource>> m_renderTargets;
//private:
//	PancyDx12DeviceBasic(HWND hwnd_window, uint32_t width_in, uint32_t height_in);
//public:
//	static PancyDx12DeviceBasic* d3dbasic_instance;
//	static luna::LResult SingleCreate(HWND hwnd_window_in, UINT wind_width_in, UINT wind_hight_in)
//	{
//		if (d3dbasic_instance != NULL)
//		{
//			return luna::g_Succeed;
//		}
//		else
//		{
//			d3dbasic_instance = new PancyDx12DeviceBasic(hwnd_window_in, wind_width_in, wind_hight_in);
//			luna::LResult check_failed = d3dbasic_instance->Init();
//			return check_failed;
//		}
//	}
//	static PancyDx12DeviceBasic* GetInstance()
//	{
//		return d3dbasic_instance;
//	}
//	luna::LResult Init();
//	luna::LResult ResetScreen(uint32_t window_width_in, uint32_t window_height_in);
//	//获取com资源信息
//	inline ID3D12Device* GetD3dDevice()
//	{
//		return m_device.Get();
//	};
//	inline ID3D12CommandQueue* GetCommandQueueDirect()
//	{
//		return command_queue_direct.Get();
//	}
//	inline ID3D12CommandQueue* GetCommandQueueCopy()
//	{
//		return command_queue_copy.Get();
//	}
//	inline ID3D12CommandQueue* GetCommandQueueCompute()
//	{
//		return command_queue_compute.Get();
//	}
//	inline luna::LResult SwapChainPresent(
//		/* [in] */ UINT SyncInterval,
//		/* [in] */ UINT Flags)
//	{
//		HRESULT hr;
//		hr = dx12_swapchain->Present(SyncInterval, Flags);
//		if (FAILED(hr))
//		{
//			luna::LResult error_message;
//			LunarDebugLogError(E_FAIL, "swapchain present error", error_message);
//			return error_message;
//		}
//		last_frame_use = current_frame_use;
//		current_frame_use = dx12_swapchain->GetCurrentBackBufferIndex();
//		return luna::g_Succeed;
//	}
//	inline ID3D12Resource* GetBackBuffer(CD3DX12_CPU_DESCRIPTOR_HANDLE& heap_handle)
//	{
//		auto now_frame_use = dx12_swapchain->GetCurrentBackBufferIndex();
//		auto rtv_offset = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), now_frame_use, rtv_offset);
//		heap_handle = rtvHandle;
//		return m_renderTargets[now_frame_use].Get();
//	}
//
//	//获取帧数字
//	inline UINT GetFrameNum()
//	{
//		return FrameCount;
//	}
//	inline UINT GetNowFrame()
//	{
//		return current_frame_use;
//	}
//	inline UINT GetLastFrame()
//	{
//		return last_frame_use;
//	}
//	void FlushGpu();
//private:
//	void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
//};


