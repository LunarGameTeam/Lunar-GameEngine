#include"LDirectX12Device.h"
#include"PancyTimeBasic.h"
#include "LDirectX12Buffer.h"
#include "LDirectX12Texture.h"
#include "LDirectX12Pipeline.h"
#include "LDirectX12Fence.h"
#include "LDirectX12CommondList.h"
#include "LDirectX12SwapChain.h"
#include "core/core_module.h"
#pragma comment ( lib, "D3D12.lib")
#pragma comment ( lib, "DXGI.lib")
using namespace luna;
using Microsoft::WRL::ComPtr;
luna::ILunarDirectXGraphicDeviceCore::ILunarDirectXGraphicDeviceCore()
{
}
luna::ILunarDirectXGraphicDeviceCore::~ILunarDirectXGraphicDeviceCore()
{

}
LResult luna::ILunarDirectXGraphicDeviceCore::InitDeviceData()
{
	HRESULT hr;
	LResult check_error;
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif
	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgi_factory));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "Create DXGIFactory Error When init D3D basic", error_message);
		return error_message;
	}
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	GetHardwareAdapter(m_dxgi_factory.Get(), &hardwareAdapter);
	hr = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "Create Dx12 Device Error When init D3D basic", error_message);
		return error_message;
	}
	//注册全局反射
	AddDirectXEnumVariable();
	return luna::g_Succeed;
}
void luna::ILunarDirectXGraphicDeviceCore::AddDirectXEnumVariable()
{
	InitRootSignatureJsonReflect();
	InitPipelineJsonReflect();
	InitTextureJsonReflect();
	InitBufferJsonReflect();
}
uint8_t* luna::ILunarDirectXGraphicDeviceCore::GetPointerFromSharedMemory(LSharedObject* dynamic_buffer_pointer)
{
	LDx12GraphicResourceBuffer* buffer_pointer = dynamic_cast<LDx12GraphicResourceBuffer*>(dynamic_buffer_pointer);
	if (buffer_pointer == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not exchange object to directx12buffer", error_message);
		return nullptr;
	}
	if (buffer_pointer->GetMapPointer() == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not get memory pointer from dx12buffer, maybe it's not a dynamic buffer", error_message);
		return nullptr;
	}
	return buffer_pointer->GetMapPointer();
}
void luna::ILunarDirectXGraphicDeviceCore::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			//跳过CPU渲染过程
			continue;
		}
		//检验是否支持dx12
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}
	*ppAdapter = adapter.Detach();
}
LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateShaderFromFile(
	const LString& file_name,
	const Json::Value& root_value,
	const bool if_check_repeat
)
{
	return LCreateAssetByJson<LDx12GraphicResourceShaderBlob>(file_name, root_value, if_check_repeat);
};
LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateShaderFromMemory(const void* Pipeline_desc_message, const bool if_check_repeat)
{
	LResult error_message;
	LunarDebugLogError(0, "unsupport memory serialize ", error_message);
	return nullptr;
}

LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateRootSignatureFromFile(
	const LString& file_name,
	const Json::Value& root_value,
	const bool if_check_repeat
)
{
	return LCreateAssetByJson<LDx12GraphicResourceRootSignature>(file_name, root_value, if_check_repeat);
};
LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateRootSignatureFromMemory(const void* Pipeline_desc_message, const bool if_check_repeat)
{
	LResult error_message;
	LunarDebugLogError(0, "unsupport memory serialize ", error_message);
	return nullptr;
};
LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreatePipelineFromFile(
	const LString& file_name,
	const Json::Value& root_value,
	const bool if_check_repeat
)
{
	return LCreateAssetByJson<LDx12GraphicResourceShaderBlob>(file_name, root_value, if_check_repeat);
};
LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreatePipelineFromMemory(const void* Pipeline_desc_message, const bool if_check_repeat)
{
	LResult error_message;
	LunarDebugLogError(0, "unsupport memory serialize ", error_message);
	return nullptr;
};

LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateBufferResourceFromFile(
	const LString& file_name,
	const Json::Value& root_value,
	const bool if_check_repeat
)
{
	return LCreateAssetByJson<LDx12GraphicResourceBuffer>(file_name, root_value, if_check_repeat);
};
LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateBufferResourceFromMemory(const void* Pipeline_desc_message, const bool if_check_repeat)
{
	LResult error_message;
	LunarDebugLogError(0, "unsupport memory serialize ", error_message);
	return nullptr;
};

LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateUniforBuffer(const size_t& uniform_buffer_size)
{
	LunaCommonBufferDesc required_uinform_buffer_desc;
	required_uinform_buffer_desc.buffer_data_file = "";
	required_uinform_buffer_desc.buffer_type = Buffer_Constant;
	required_uinform_buffer_desc.buffer_res_desc.Alignment = 0;
	required_uinform_buffer_desc.buffer_res_desc.DepthOrArraySize = 1;
	required_uinform_buffer_desc.buffer_res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	required_uinform_buffer_desc.buffer_res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	required_uinform_buffer_desc.buffer_res_desc.Height = 1;
	required_uinform_buffer_desc.buffer_res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	required_uinform_buffer_desc.buffer_res_desc.MipLevels = 1;
	required_uinform_buffer_desc.buffer_res_desc.SampleDesc.Count = 1;
	required_uinform_buffer_desc.buffer_res_desc.SampleDesc.Quality = 0;
	required_uinform_buffer_desc.buffer_res_desc.Width = uniform_buffer_size;
	return LCreateAssetByDesc<LDx12GraphicResourceBuffer>("UniformBuffer", required_uinform_buffer_desc, false);
}

LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateTextureResourceFromFile(
	const LString& file_name,
	const Json::Value& root_value,
	const bool if_check_repeat
)
{
	return LCreateAssetByJson<LDx12GraphicResourceTexture>(file_name, root_value, if_check_repeat);
};
LBasicAsset* luna::ILunarDirectXGraphicDeviceCore::CreateTextureResourceFromMemory(const void* Pipeline_desc_message, const bool if_check_repeat)
{
	LResult error_message;
	LunarDebugLogError(0, "unsupport memory serialize ", error_message);
	return nullptr;
};

ILunarGraphicRenderCommondList* luna::ILunarDirectXGraphicDeviceCore::CreateCommondList(
	LSharedObject* allocator,
	LSharedObject* pipeline,
	const LunarGraphicPipeLineType& pipeline_type
)
{
	ILunarGraphicRenderCommondList* new_object = LCreateObject<LDx12GraphicRenderCommondList>(pipeline_type);
	LResult check_error = new_object->Create(allocator, pipeline);
	if (!check_error.m_IsOK)
	{
		return nullptr;
	}
	return new_object;
};
ILunarGraphicRenderCommondAllocator* luna::ILunarDirectXGraphicDeviceCore::CreateCommondAllocator(const LunarGraphicPipeLineType& pipeline_type)
{
	ILunarGraphicRenderCommondAllocator* new_object = LCreateObject<LDx12GraphicRenderCommondAllocator>(pipeline_type);
	LResult check_error = new_object->Create();
	if (!check_error.m_IsOK)
	{
		return nullptr;
	}
	return new_object;
};

ILunarGraphicRenderFence* luna::ILunarDirectXGraphicDeviceCore::CreateFence()
{
	ILunarGraphicRenderFence* new_object = LCreateObject<LDx12GraphicRenderFence>();
	LResult check_error = new_object->Create();
	if (!check_error.m_IsOK)
	{
		return nullptr;
	}
	return new_object;
};


luna::LResult ILunarGraphicDeviceCore::SingleCreate()
{
	luna::LResult check_error;
	_graphic_device_instance = new ILunarDirectXGraphicDeviceCore();
	check_error = _graphic_device_instance->Create();
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}

//PancyDx12DeviceBasic* PancyDx12DeviceBasic::d3dbasic_instance = NULL;
//
//PancyDx12DeviceBasic::PancyDx12DeviceBasic(HWND hwnd_window_in, uint32_t width_in, uint32_t height_in)
//{
//	FrameCount = 2;
//	for (uint32_t i = 0; i < FrameCount; ++i)
//	{
//		m_renderTargets.push_back(NULL);
//	}
//	width = width_in;
//	height = height_in;
//	hwnd_window = hwnd_window_in;
//	//Direct_queue_fence_value.store(0);
//}
//LResult PancyDx12DeviceBasic::Init()
//{
//	HRESULT hr;
//	LResult check_error;
//	UINT dxgiFactoryFlags = 0;
//#if defined(_DEBUG)
//	{
//		ComPtr<ID3D12Debug> debugController;
//		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//		{
//			debugController->EnableDebugLayer();
//			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//		}
//	}
//#endif
//	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgi_factory));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create DXGIFactory Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	ComPtr<IDXGIAdapter1> hardwareAdapter;
//	GetHardwareAdapter(dxgi_factory.Get(), &hardwareAdapter);
//	hr = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Dx12 Device Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	//创建直接渲染队列
//	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//	hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue_direct));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Command Queue(Direct) Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	command_queue_direct->SetName(LString("direct queue").GetStdUnicodeString().c_str());
//	//创建拷贝渲染队列
//	D3D12_COMMAND_QUEUE_DESC queueDesc_copy = {};
//	queueDesc_copy.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	queueDesc_copy.Type = D3D12_COMMAND_LIST_TYPE_COPY;
//	hr = m_device->CreateCommandQueue(&queueDesc_copy, IID_PPV_ARGS(&command_queue_copy));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Command Queue(Copy) Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	command_queue_direct->SetName(LString("copy queue").GetStdUnicodeString().c_str());
//	//创建计算渲染队列
//	D3D12_COMMAND_QUEUE_DESC queueDesc_compute = {};
//	queueDesc_compute.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	queueDesc_compute.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
//	hr = m_device->CreateCommandQueue(&queueDesc_compute, IID_PPV_ARGS(&command_queue_compute));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Command Queue(Compute) Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	command_queue_direct->SetName(LString("Compute queue").GetStdUnicodeString().c_str());
//	//创建用于最终销毁队列时，检查queue状态的fence
//	//创建用于接收gpu同步信息的fence
//	hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&queue_fence_direct));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Fence Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&queue_fence_compute));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Fence Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	hr = PancyDx12DeviceBasic::GetInstance()->GetD3dDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&queue_fence_copy));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Fence Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	//创建屏幕
//	check_error = ResetScreen(width, height);
//	if (!check_error.m_IsOK)
//	{
//		return check_error;
//	}
//	//创建一个资源堆
//	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
//	rtvHeapDesc.NumDescriptors = FrameCount;
//	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create DescriptorHeap Error When init D3D basic", error_message);
//		return error_message;
//
//	}
//	//获取资源堆的偏移大小
//	auto rtv_offset = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//	//在资源堆上创建RTV
//	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
//	for (UINT n = 0; n < FrameCount; n++)
//	{
//		hr = dx12_swapchain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
//		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
//		rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
//		rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
//		m_device->CreateRenderTargetView(m_renderTargets[n].Get(), &rtv_desc, rtvHandle);
//		rtvHandle.Offset(1, rtv_offset);
//	}
//	//禁止alt+回车全屏
//	dxgi_factory->MakeWindowAssociation(hwnd_window, DXGI_MWA_NO_ALT_ENTER);
//	//检查支持的格式
//	D3D12_FEATURE_DATA_D3D12_OPTIONS formatInfo = { };
//	m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &formatInfo, sizeof(formatInfo));
//	return g_Succeed;
//}
//LResult PancyDx12DeviceBasic::ResetScreen(uint32_t window_width_in, uint32_t window_height_in)
//{
//	HRESULT hr;
//	//重新创建交换链
//	if (dx12_swapchain != NULL)
//	{
//		dx12_swapchain.Reset();
//	}
//	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
//	swapChainDesc.BufferCount = FrameCount;
//	swapChainDesc.Width = width;
//	swapChainDesc.Height = height;
//	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//	swapChainDesc.SampleDesc.Count = 1;
//	ComPtr<IDXGISwapChain1> swapChain;
//	hr = dxgi_factory->CreateSwapChainForHwnd(
//		command_queue_direct.Get(),
//		hwnd_window,
//		&swapChainDesc,
//		nullptr,
//		nullptr,
//		&swapChain
//	);
//	if (FAILED(hr))
//	{
//		LResult error_message;
//		LunarDebugLogError(hr, "Create Swap Chain Error When init D3D basic", error_message);
//
//		return error_message;
//	}
//	swapChain.As(&dx12_swapchain);
//	last_frame_use = dx12_swapchain->GetCurrentBackBufferIndex();
//	current_frame_use = dx12_swapchain->GetCurrentBackBufferIndex();
//	return g_Succeed;
//}
//void PancyDx12DeviceBasic::FlushGpu()
//{
//	uint64_t fenceValueForSignal = 1;
//	//等待渲染队列结束
//	HANDLE wait_thread_ID_direct;
//	wait_thread_ID_direct = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//	if (wait_thread_ID_direct == nullptr)
//	{
//		LResult error_message;
//		LunarDebugLogError(E_FAIL, "Create fence event Error When build commandlist", error_message);
//
//	}
//	command_queue_direct->Signal(queue_fence_direct.Get(), fenceValueForSignal);
//	if (queue_fence_direct->GetCompletedValue() < 1)
//	{
//		queue_fence_direct->SetEventOnCompletion(fenceValueForSignal, wait_thread_ID_direct);
//		WaitForSingleObject(wait_thread_ID_direct, INFINITE);
//	}
//	CloseHandle(wait_thread_ID_direct);
//	//等待计算队列结束
//	HANDLE wait_thread_ID_compute;
//	wait_thread_ID_compute = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//	if (wait_thread_ID_compute == nullptr)
//	{
//		LResult error_message;
//		LunarDebugLogError(E_FAIL, "Create fence event Error When build commandlist", error_message);
//
//	}
//	command_queue_compute->Signal(queue_fence_compute.Get(), fenceValueForSignal);
//	if (queue_fence_compute->GetCompletedValue() < 1)
//	{
//		queue_fence_compute->SetEventOnCompletion(fenceValueForSignal, wait_thread_ID_compute);
//		WaitForSingleObject(wait_thread_ID_compute, INFINITE);
//	}
//	CloseHandle(wait_thread_ID_compute);
//	//等待拷贝队列结束
//	HANDLE wait_thread_ID_copy;
//	wait_thread_ID_copy = CreateEvent(nullptr, FALSE, FALSE, nullptr);
//	if (wait_thread_ID_copy == nullptr)
//	{
//		LResult error_message;
//		LunarDebugLogError(E_FAIL, "Create fence event Error When build commandlist", error_message);
//
//	}
//	command_queue_copy->Signal(queue_fence_copy.Get(), fenceValueForSignal);
//	if (queue_fence_copy->GetCompletedValue() < 1)
//	{
//		queue_fence_copy->SetEventOnCompletion(fenceValueForSignal, wait_thread_ID_copy);
//		WaitForSingleObject(wait_thread_ID_copy, INFINITE);
//	}
//	CloseHandle(wait_thread_ID_copy);
//}
//void PancyDx12DeviceBasic::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
//{
//	ComPtr<IDXGIAdapter1> adapter;
//	*ppAdapter = nullptr;
//	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
//	{
//		DXGI_ADAPTER_DESC1 desc;
//		adapter->GetDesc1(&desc);
//		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
//		{
//			//跳过CPU渲染过程
//			continue;
//		}
//		//检验是否支持dx12
//		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
//		{
//			break;
//		}
//	}
//	*ppAdapter = adapter.Detach();
//}



