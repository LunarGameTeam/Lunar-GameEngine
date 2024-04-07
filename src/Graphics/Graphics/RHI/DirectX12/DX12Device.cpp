#include "DX12Device.h"
#include "DX12Pipeline.h"
#include "DX12Fence.h"
#include "DX12CommandList.h"
#include "DX12Swapchain.h"
#include "DX12Memory.h"
#include "DX12DescriptorPool.h"
#include "DX12DescriptorImpl.h"
#include "DX12Resource.h"

#include "DX12RenderPass.h"
#include "Graphics/RHI/RHIFrameBuffer.h"

#include "Core/CoreMin.h"

#include "Graphics/RHI/DirectX12/DX12BindingSet.h"
#include "DX12View.h"

using Microsoft::WRL::ComPtr;


namespace luna::graphics
{


DX12MemoryManagerPool::DX12MemoryManagerPool()
{

}

DX12MemoryManagerPool::~DX12MemoryManagerPool()
{
	mAllocator->Release();
}

bool DX12MemoryManagerPool::Create(const D3D12MA::ALLOCATOR_DESC& allocatorDesc)
{
	HRESULT hr = D3D12MA::CreateAllocator(&allocatorDesc, &mAllocator);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool DX12MemoryManagerPool::BindResourceMemory(RHIHeapType type, const D3D12_RESOURCE_DESC& memoryRequire, Microsoft::WRL::ComPtr<ID3D12Resource>& resource, D3D12MA::Allocation*& allocation)
{
	D3D12MA::ALLOCATION_DESC allocationDesc = {};
	if (type == RHIHeapType::Default)
	{
		allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
	}
	else if (type == RHIHeapType::Upload)
	{
		allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
	}
	else
	{
		assert(false);
	}
	HRESULT hr = mAllocator->CreateResource(
		&allocationDesc,
		&memoryRequire,
		D3D12_RESOURCE_STATE_COMMON,
		NULL,
		&allocation,
		IID_PPV_ARGS(&resource));
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool DX12MemoryManagerPool::FreeResourceMemory(D3D12MA::Allocation*& allocation)
{
	if (allocation != nullptr) 
	{
		allocation->Release();
	}
	return true;
};

DX12Device::DX12Device()
{

}

void DX12Device::InitDescriptorHeap()
{
	TRHIPtr<Dx12GpuDescriptorHeap> srv_gpu_descriptor = CreateRHIObject<Dx12GpuDescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		4096*6
		);
	srv_gpu_descriptor->Init();
	TRHIPtr<Dx12GpuDescriptorHeap> sampler_gpu_descriptor = CreateRHIObject<Dx12GpuDescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		2048
		);
	sampler_gpu_descriptor->Init();
	TRHIPtr<Dx12CpuDescriptorHeap> srv_cpu_descriptor = CreateRHIObject<Dx12CpuDescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		4096
		);
	srv_cpu_descriptor->Init();
	TRHIPtr<Dx12CpuDescriptorHeap> sampler_cpu_descriptor = CreateRHIObject<Dx12CpuDescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		2048
		);
	sampler_cpu_descriptor->Init();
	TRHIPtr<Dx12CpuDescriptorHeap> rtv_cpu_descriptor = CreateRHIObject<Dx12CpuDescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		2048
		);
	rtv_cpu_descriptor->Init();
	TRHIPtr<Dx12CpuDescriptorHeap> dsv_cpu_descriptor = CreateRHIObject<Dx12CpuDescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		2048
		);
	dsv_cpu_descriptor->Init();
	gpu_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = srv_gpu_descriptor;
	gpu_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = sampler_gpu_descriptor;

	cpu_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = srv_cpu_descriptor;
	cpu_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = sampler_cpu_descriptor;
	cpu_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = rtv_cpu_descriptor;
	cpu_descriptor_heap[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = dsv_cpu_descriptor;
}

DX12Device::~DX12Device()
{
}

bool DX12Device::CheckIfResourceHeapTire2()
{
	if (m_feature_desc.ResourceHeapTier == D3D12_RESOURCE_HEAP_TIER_2)
	{
		return true;
	}
	return false;
}

bool DX12Device::InitDeviceData()
{
	HRESULT hr;
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

		LogError("Device", "Create DXGIFactory Error When init D3D basic");
		return false;

	}
	GetHardwareAdapter(m_dxgi_factory.Get(), &mHardwareAdapter);
	hr = D3D12CreateDevice(mHardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr))
	{

		LogError("Device", "Create Dx12 Device Error When init D3D basic");
		return false;
	}
	//注册全局反射
	AddDirectXEnumVariable();
	m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &m_feature_desc, sizeof(m_feature_desc));

	InitDescriptorHeap();
	D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
	allocatorDesc.pDevice = m_device.Get();
	allocatorDesc.pAdapter = mHardwareAdapter.Get();
	mDefaultDmaMemoryAllocator.Create(allocatorDesc);
	return  true;
}

void DX12Device::AddDirectXEnumVariable()
{
}

void DX12Device::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
	ComPtr<IDXGIAdapter1> adapter;
	*ppAdapter = nullptr;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++
		adapterIndex)
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

luna::graphics::RHIShaderBlobPtr DX12Device::CreateShader(const RHIShaderDesc& desc)
{
	return CreateRHIObject<DX12ShaderBlob>(desc);
}

RHIPipelineStatePtr DX12Device::CreatePipelineGraphic(
	const RHIPipelineStateGraphDrawDesc& desc,
	const RHIVertexLayout& inputLayout,
	const RenderPassDesc& renderPassDesc
)
{
	LSharedPtr<RHIPipelineStateGraphDrawDesc> curDesc = MakeShared<RHIPipelineStateGraphDrawDesc>();
	*curDesc = desc;
	luna::graphics::RHIPipelineStatePtr dx12Pipeline = CreateRHIObject<DX12PipelineStateGraphic>(curDesc, inputLayout, renderPassDesc);
	dx12Pipeline->Create(this);
	return dx12Pipeline;
}

RHIPipelineStatePtr DX12Device::CreatePipelineCompute(const RHIPipelineStateComputeDesc& desc)
{
	LSharedPtr<RHIPipelineStateComputeDesc> curDesc = MakeShared<RHIPipelineStateComputeDesc>();
	*curDesc = desc;
	luna::graphics::RHIPipelineStatePtr dx12Pipeline = CreateRHIObject<DX12PipelineStateCompute>(curDesc);
	dx12Pipeline->Create(this);
	return dx12Pipeline;
}


RHICmdSignaturePtr DX12Device::CreateCmdSignature(RHIPipelineState* pipeline, const LArray<CommandArgDesc>& allCommondDesc)
{
	return CreateRHIObject<DX12CmdSignature>(pipeline, allCommondDesc);
}

RHIBindingSetLayoutPtr DX12Device::CreateBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys, const std::unordered_map<ShaderParamID, RHIPushConstantValue>& mBindConstKeys)
{
	return CreateRHIObject<DX12BindingSetLayout>(bindKeys, mBindConstKeys);
}

luna::graphics::TRHIPtr<luna::graphics::RHIMemory> DX12Device::AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits /*= 0*/)
{
	return CreateRHIObject<DX12Memory>(desc);;
}

luna::graphics::RHIDescriptorPoolPtr DX12Device::CreateDescriptorPool(const DescriptorPoolDesc& desc)
{
	return CreateRHIObject<DX12DescriptorPool>(desc);
}

RHISinglePoolSingleCmdListPtr DX12Device::CreateSinglePoolSingleCommondList(RHICmdListType type)
{
	return CreateRHIObject<DX12SinglePoolSingleCmdList>(type);
}

RHISinglePoolMultiCmdListPtr DX12Device::CreateSinglePoolMultiCommondList(RHICmdListType type)
{
	return CreateRHIObject<DX12SinglePoolMultiCmdList>(type);
}

RHIMultiFrameCmdListPtr DX12Device::CreateMultiFrameCommondList(size_t frameCount, RHICmdListType type)
{
	return CreateRHIObject<DX12MultiFrameCmdList>(frameCount,type);
}

RHIViewPtr DX12Device::CreateDescriptor(
	const RHIViewType& descriptor_type,
	size_t descriptor_offset,
	size_t descriptor_size,
	RHIDescriptorPool* descriptor_heap
)
{
	return nullptr;
}

TRHIPtr<RHIFence> DX12Device::CreateFence()
{
	return CreateRHIObject<DX12Fence>();
};

size_t DX12Device::CountResourceSizeByDesc(const RHIResDesc& res_desc)
{
	auto dx12_res_desc = GetResourceDesc(res_desc);
	auto res_allocation_info = m_device->GetResourceAllocationInfo(0, 1, &dx12_res_desc);
	return res_allocation_info.SizeInBytes;
};

RHIFrameBufferPtr DX12Device::CreateFrameBuffer(const FrameBufferDesc& desc)
{
	return CreateRHIObject<DX12FrameBuffer>(desc);
}

RHIRenderPassPtr DX12Device::CreateRenderPass(const RenderPassDesc& desc)
{
	return CreateRHIObject<Dx12RenderPass>(desc);;
}

RHIResourcePtr DX12Device::CreateSamplerExt(const SamplerDesc& desc)
{
	return CreateRHIObject<DX12Resource>(desc);
}

RHIViewPtr DX12Device::CreateView(const ViewDesc& desc)
{
	return CreateRHIObject<DX12View>(desc);
}

RHIBindingSetPtr DX12Device::CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout)
{
	return CreateRHIObject<DX12BindingSet>(pool, layout);
}

luna::graphics::RHIResourcePtr DX12Device::CreateTextureExt(const RHIResDesc& resDesc)
{
	return CreateRHIObject<DX12Resource>(resDesc);
}

RHIResourcePtr DX12Device::CreateBufferExt(const RHIBufferDesc& bufferDesc)
{
	return CreateRHIObject<DX12Resource>(bufferDesc);
}

}
