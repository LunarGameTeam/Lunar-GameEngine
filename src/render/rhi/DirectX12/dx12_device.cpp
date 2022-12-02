#include "dx12_device.h"
#include "dx12_pipeline.h"
#include "dx12_fence.h"
#include "dx12_command_list.h"
#include "dx12_swapchain.h"
#include "dx12_memory.h"
#include "dx12_descriptor_pool.h"
#include "dx12_descriptor_impl.h"
#include "dx12_resource.h"
#include "dx12_upload_ringbuffer.h"
#include "dx12_render_pass.h"
#include "render/rhi/rhi_frame_buffer.h"

#include "core/core_library.h"

#include "render/rhi/DirectX12/dx12_binding_set.h"
#include "dx12_view.h"

using Microsoft::WRL::ComPtr;


namespace luna::render
{



DX12Device::DX12Device()
{

}

void DX12Device::InitDescriptorHeap()
{
	TRHIPtr<Dx12GpuDescriptorHeap> srv_gpu_descriptor = CreateRHIObject<Dx12GpuDescriptorHeap>(
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		4096
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
	gpu_descriptor_heap.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srv_gpu_descriptor);
	gpu_descriptor_heap.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, sampler_gpu_descriptor);

	cpu_descriptor_heap.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srv_cpu_descriptor);
	cpu_descriptor_heap.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, sampler_cpu_descriptor);
	cpu_descriptor_heap.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtv_cpu_descriptor);
	cpu_descriptor_heap.emplace(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, dsv_cpu_descriptor);
}

DX12Device::~DX12Device()
{
	if (m_upload_buffer != nullptr)
	{
		delete m_upload_buffer;
	}
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
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	GetHardwareAdapter(m_dxgi_factory.Get(), &hardwareAdapter);
	hr = D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
	if (FAILED(hr))
	{

		LogError("Device", "Create Dx12 Device Error When init D3D basic");
		return false;
	}
	//注册全局反射
	AddDirectXEnumVariable();
	m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &m_feature_desc, sizeof(m_feature_desc));
	m_upload_buffer = new LDirectx12DynamicRingBuffer();

	InitDescriptorHeap();
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

luna::render::RHIShaderBlobPtr DX12Device::CreateShader(const RHIShaderDesc& desc)
{
	return CreateRHIObject<DX12ShaderBlob>(desc);
}

luna::render::RHIPipelineStatePtr DX12Device::CreatePipeline(const RHIPipelineStateDesc& desc)
{
	return CreateRHIObject<DX12PipelineState>(desc);
};

RHIBindingSetLayoutPtr DX12Device::CreateBindingSetLayout(const std::vector<RHIBindPoint>& bindKeys)
{
	return CreateRHIObject<DX12BindingSetLayout>(bindKeys);
}

luna::render::TRHIPtr<luna::render::RHIMemory> DX12Device::AllocMemory(const RHIMemoryDesc& desc, uint32_t memoryBits /*= 0*/)
{
	return CreateRHIObject<DX12Memory>(desc);;
}

void DX12Device::CopyInitDataToResource(void* initData, size_t dataSize, RHIResourcePtr sourceDataLayout, RHIResourcePtr resDynamic)
{
	
// 	dxMemoryLayout->mDxRequire.pLayouts.resize(mDxDesc.DepthOrArraySize);
// 	dxMemoryLayout->mDxRequire.pNumRows.resize(mDxDesc.DepthOrArraySize);
// 	dxMemoryLayout->mDxRequire.pRowSizeInBytes.resize(mDxDesc.DepthOrArraySize);
// 	device->GetCopyableFootprints(
// 		&mDxDesc,
// 		0,
// 		mDxDesc.DepthOrArraySize,
// 		0,
// 		dxMemoryLayout->mDxRequire.pLayouts.data(),
// 		dxMemoryLayout->mDxRequire.pNumRows.data(),
// 		dxMemoryLayout->mDxRequire.pRowSizeInBytes.data(),
// 		&dxMemoryLayout->mDxRequire.pTotalBytes
// 	);

// 	DX12ResourceNew* dx12DstRes = resDynamic->As<DX12ResourceNew>();
// 	const DxResourceMemorylayout* dx12MemoryLayout = static_cast<const DxResourceMemorylayout*>(sourceDataLayout);
// 	const DxResourceCopyLayout& srcDx12CopyLayout = dx12MemoryLayout->mDxRequire;
// 	if (sourceDataLayout->mBaseRequire.size > dataSize)
// 	{
// 		LogWarning("Device", "copy init data to upload buffer May Failed");
// 		memcpy(dx12DstRes->Map(), initData, dataSize);
// 		return;
// 	}
// 	LVector<D3D12_SUBRESOURCE_DATA> subs(srcDx12CopyLayout.pLayouts.size());
// 	for (int i = 0; i < srcDx12CopyLayout.pLayouts.size(); i++)
// 	{
// 		D3D12_SUBRESOURCE_DATA& it = subs[i];
// 		
// 		it.pData = (byte*)(initData)+i * srcDx12CopyLayout.pNumRows[i] * srcDx12CopyLayout.pRowSizeInBytes[i];
// 		it.RowPitch = srcDx12CopyLayout.pRowSizeInBytes[i];
// 		it.SlicePitch = srcDx12CopyLayout.pNumRows[i] * srcDx12CopyLayout.pRowSizeInBytes[i];
// 
// 		D3D12_MEMCPY_DEST DestData = {
// 			(byte*)dx12DstRes->Map() + srcDx12CopyLayout.pLayouts[i].Offset,
// 			srcDx12CopyLayout.pLayouts[i].Footprint.RowPitch,
// 			srcDx12CopyLayout.pLayouts[i].Footprint.RowPitch * srcDx12CopyLayout.pNumRows[i]
// 		};
// 		MemcopyDataToResource(&DestData, &it, (SIZE_T)srcDx12CopyLayout.pRowSizeInBytes[i], srcDx12CopyLayout.pNumRows[i],
// 			srcDx12CopyLayout.pLayouts[i].Footprint.Depth);
// 	}
}

luna::render::RHIDescriptorPoolPtr DX12Device::CreateDescriptorPool(DescriptorPoolDesc desc)
{
	return CreateRHIObject<DX12DescriptorPool>(desc);
}

RHIGraphicCmdListPtr DX12Device::CreateCommondList(RHICmdListType pipeline_type)
{
	return CreateRHIObject<DX12GraphicCmdList>(pipeline_type);
};

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

RHIResourcePtr DX12Device::CreateUniforBuffer(size_t uniform_buffer_size)
{
	RHIResDesc required_uinform_buffer_desc;
	required_uinform_buffer_desc.ResHeapType = RHIHeapType::Upload;
	required_uinform_buffer_desc.Desc.Alignment = 0;
	required_uinform_buffer_desc.Desc.DepthOrArraySize = 1;
	required_uinform_buffer_desc.Desc.Dimension = RHIResDimension::Buffer;
	required_uinform_buffer_desc.Desc.mUsage = RHIResourceUsage::RESOURCE_FLAG_NONE;
	required_uinform_buffer_desc.Desc.Height = 1;
	required_uinform_buffer_desc.Desc.Layout = RHITextureLayout::TEXTURE_LAYOUT_ROW_MAJOR;
	required_uinform_buffer_desc.Desc.MipLevels = 1;
	required_uinform_buffer_desc.Desc.SampleDesc.Count = 1;
	required_uinform_buffer_desc.Desc.SampleDesc.Quality = 0;
	required_uinform_buffer_desc.Desc.Width = uniform_buffer_size;
	auto p = CreateRHIObject<DX12Resource>(required_uinform_buffer_desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
	return p;
}

luna::render::RHIResourcePtr DX12Device::CreateCommitTexture(
	RHITextureDesc& texture_desc,
	RHIResDesc& texture_resource_desc,
	byte* init_data,
	size_t data_size,
	TextureMemoryType init_memory_type
)
{
	RHIResourcePtr empty_pointer;

	if (init_memory_type == TextureMemoryType::DDS)
	{
		return empty_pointer;
	}

	empty_pointer = CreateRHIObject<DX12Resource>(texture_resource_desc, 1,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
	TRHIPtr<RHIResource> now_res_out = empty_pointer;
	//根据资源格式确定资源的拷贝信息
	auto res_desc_dx = GetResourceDesc(texture_resource_desc.Desc);
	auto depth = res_desc_dx.DepthOrArraySize;
	if (depth == 1)
	{
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT copy_layout;
		UINT pNumRows;
		UINT64 pRowSizeInBytes;
		UINT64 pTotalBytes;
		m_device->GetCopyableFootprints(&res_desc_dx, 0, 1, 0, &copy_layout, &pNumRows, &pRowSizeInBytes,
			&pTotalBytes);
		auto footprint_use = GetFootPrintDx(copy_layout);
		assert(pTotalBytes == data_size);
		m_upload_buffer->CopyDataToGpu(init_data, data_size, now_res_out, footprint_use, 0);
		return empty_pointer;
	}
	if (depth > 1)
	{

		LVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> copy_layout(depth);
		LVector<D3D12_SUBRESOURCE_DATA> subs(depth);
		LVector<UINT> pNumRows(depth);
		LVector<UINT64> pRowSizeInBytes(depth);
		UINT64 pTotalBytes;
		m_device->GetCopyableFootprints(&res_desc_dx, 0, depth, 0, copy_layout.data(), pNumRows.data(), pRowSizeInBytes.data(),
			&pTotalBytes);
		for (int i = 0; i < depth; i++)
		{
			D3D12_SUBRESOURCE_DATA& it = subs[i];
			it.pData = (byte*)(init_data)+i * pNumRows[i] * pRowSizeInBytes[i];
			it.RowPitch = pRowSizeInBytes[i];
			it.SlicePitch = pNumRows[i] * pRowSizeInBytes[i];
		}
		m_upload_buffer->CopyDataToGpu(subs, copy_layout, pRowSizeInBytes, pNumRows, now_res_out, data_size);

		return empty_pointer;
	}
	return empty_pointer;
}

RHIResourcePtr DX12Device::CreateTextureArray(RHITextureDesc& texture_desc, RHIResDesc& texture_resource_desc, LVector<const byte*> init_datas, TextureMemoryType init_memory_type)
{

	RHIResourcePtr empty_pointer;
	if (init_memory_type == TextureMemoryType::DDS)
	{
		return empty_pointer;
	}

	empty_pointer = CreateRHIObject<DX12Resource>(texture_resource_desc, 1,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
	TRHIPtr<RHIResource> now_res_out = empty_pointer;
	//根据资源格式确定资源的拷贝信息
	auto res_desc_dx = GetResourceDesc(texture_resource_desc.Desc);
	auto depth = res_desc_dx.DepthOrArraySize;
	assert(depth > 1);
	assert(depth == init_datas.size());
	LVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> copy_layout(depth);
	LVector<D3D12_SUBRESOURCE_DATA> subs(depth);
	LVector<UINT> pNumRows(depth);
	LVector<UINT64> pRowSizeInBytes(depth);
	UINT64 pTotalBytes;
	m_device->GetCopyableFootprints(&res_desc_dx, 0, depth, 0, copy_layout.data(), pNumRows.data(), pRowSizeInBytes.data(),
		&pTotalBytes);
	for (int i = 0; i < depth; i++)
	{
		D3D12_SUBRESOURCE_DATA& it = subs[i];
		it.pData = init_datas[i];
		it.RowPitch = pRowSizeInBytes[i];
		it.SlicePitch = pNumRows[i] * pRowSizeInBytes[i];
	}
	m_upload_buffer->CopyDataToGpu(subs, copy_layout, pRowSizeInBytes, pNumRows, now_res_out, pTotalBytes);

	return empty_pointer;

}



luna::render::RHIResourcePtr DX12Device::CreateCommitBuffer(
	const RHIBufferDesc& buffer_desc,
	void* init_data
)
{
	return nullptr;
	// 	RHIResDesc bufferDesc;
	// 	bufferDesc.Desc.Dimension = RHIResDimension::Buffer;
	// 	bufferDesc.Desc.Height = 1;
	// 	bufferDesc.Desc.DepthOrArraySize = 1;
	// 	bufferDesc.Desc.MipLevels = 1;
	// 	bufferDesc.Desc.Layout = RHITextureLayout::TEXTURE_LAYOUT_ROW_MAJOR;
	// 	RHIResourcePtr now_buffer_out = CreateRHIObject<DX12Resource>(
	// 		buffer_desc, D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
	// 	TRHIPtr<RHIResource> now_res_out = now_buffer_out;
	// 	m_upload_buffer->CopyDataToGpu(init_data, buffer_desc.Width, now_res_out);
	// 	return now_buffer_out;
};

luna::render::RHIResourcePtr DX12Device::CreatePlacedBuffer(
	RHIMemory* res_heap,
	size_t offset,
	size_t buffer_size,
	LResState res_state
)
{
	RHIResDesc res_desc;
	res_desc.ResHeapType = res_heap->GetDesc().Type;
	res_desc.Desc.Alignment = 0;
	res_desc.Desc.DepthOrArraySize = 1;
	res_desc.Desc.Dimension = RHIResDimension::Buffer;
	res_desc.Desc.mUsage = RHIResourceUsage::RESOURCE_FLAG_NONE;
	res_desc.Desc.Height = 1;
	res_desc.Desc.Layout = RHITextureLayout::TEXTURE_LAYOUT_ROW_MAJOR;
	res_desc.Desc.MipLevels = 1;
	res_desc.Desc.SampleDesc.Count = 1;
	res_desc.Desc.SampleDesc.Quality = 0;
	res_desc.Desc.Width = buffer_size;
	res_desc.Desc.Format = RHITextureFormat::FORMAT_UNKNOWN;
	return CreateRHIObject<DX12Resource>(res_heap, offset, res_desc, GetResourceState(res_state));
};

RHIResourcePtr DX12Device::CreatePlacedTexture(
	RHIMemory* res_heap,
	size_t offset,
	const RHITextureDesc& texture_desc,
	const RHIResDesc& texture_resource_desc
)
{
	size_t numberOfResources = 1;
	if (texture_resource_desc.Desc.Dimension != RHIResDimension::Texture3D)
	{
		numberOfResources = texture_resource_desc.Desc.DepthOrArraySize;
	}
	numberOfResources *= texture_resource_desc.Desc.MipLevels;
	auto format = GetGraphicFormat(texture_resource_desc.Desc.Format);
	UINT numberOfPlanes = D3D12GetFormatPlaneCount(m_device.Get(), format);
	numberOfResources *= numberOfPlanes;
	auto p = CreateRHIObject<DX12Resource>(res_heap, offset, texture_resource_desc, numberOfResources,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
	return p;
};

RHIResourcePtr DX12Device::CreateEmptyTexture(
	const RHITextureDesc& texture_desc,
	const RHIResDesc& texture_resource_desc
)
{
	size_t numberOfResources = 1;
	if (texture_resource_desc.Desc.Dimension != RHIResDimension::Texture3D)
	{
		numberOfResources = texture_resource_desc.Desc.DepthOrArraySize;
	}
	numberOfResources *= texture_resource_desc.Desc.MipLevels;
	auto format = GetGraphicFormat(texture_resource_desc.Desc.Format);
	UINT numberOfPlanes = D3D12GetFormatPlaneCount(m_device.Get(), format);
	numberOfResources *= numberOfPlanes;
	return CreateRHIObject<DX12Resource>(texture_resource_desc, numberOfResources,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON);
}

size_t DX12Device::CountResourceSizeByDesc(const RHIResDesc& res_desc)
{
	auto dx12_res_desc = GetResourceDesc(res_desc.Desc);
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
	ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();

	RHIResourcePtr res = CreateRHIObject<DX12ResourceNew>();
	res->mDimension = RHIResDimension::Unknown;
	res->mResType = ResourceType::kSampler;
	
	DX12ResourceNew* dx12Res = res->As< DX12ResourceNew>();
	D3D12_SAMPLER_DESC& sampler_desc = dx12Res->mDxSamplerDesc;

	switch (desc.filter)
	{
	case SamplerFilter::kAnisotropic:
		sampler_desc.Filter = D3D12_FILTER_ANISOTROPIC;
		break;
	case SamplerFilter::kMinMagMipLinear:
		sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case SamplerFilter::kComparisonMinMagMipLinear:
		sampler_desc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	}

	switch (desc.mode)
	{
	case SamplerTextureAddressMode::kWrap:
		sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		break;
	case SamplerTextureAddressMode::kClamp:
		sampler_desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler_desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sampler_desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		break;
	}

	switch (desc.func)
	{
	case SamplerComparisonFunc::kNever:
		sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		break;
	case SamplerComparisonFunc::kAlways:
		sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		break;
	case SamplerComparisonFunc::kLess:
		sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	}

	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = std::numeric_limits<float>::max();
	sampler_desc.MaxAnisotropy = 1;
	return res;
}

RHIViewPtr DX12Device::CreateView(const ViewDesc& desc)
{
	return CreateRHIObject<DX12View>(desc);
}

RHIBindingSetPtr DX12Device::CreateBindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout)
{
	return CreateRHIObject<DX12BindingSet>(pool, layout);
}

luna::render::RHIResourcePtr DX12Device::CreateTextureExt(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc)
{
	RHIResourcePtr res = CreateRHIObject<DX12ResourceNew>();
	res->mResType = ResourceType::kTexture;
	res->mFormat = resDesc.Desc.Format;
	res->mDimension = resDesc.Desc.Dimension;

	DX12ResourceNew* dxRes = res->As<DX12ResourceNew>();
	


	D3D12_RESOURCE_DESC& desc = dxRes->mDxDesc;
	desc.Width = resDesc.Desc.Width;
	desc.Height = resDesc.Desc.Height;
	desc.DepthOrArraySize = resDesc.Desc.DepthOrArraySize;
	desc.MipLevels = resDesc.Desc.MipLevels;

	switch (resDesc.Desc.Format)
	{
	case RHITextureFormat::FORMAT_R8G8BB8A8_UNORM:
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case RHITextureFormat::FORMAT_D24_UNORM_S8_UINT:
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}
	switch (resDesc.Desc.Dimension)
	{
	case RHIResDimension::Texture1D:
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		break;
	case RHIResDimension::Texture2D:
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		break;
	case RHIResDimension::Texture3D:
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		break;
	}

	desc.SampleDesc.Count = resDesc.Desc.SampleDesc.Count;
	desc.SampleDesc.Quality = 0;

	if (Has(resDesc.Desc.mImageUsage, RHIImageUsage::ColorAttachmentBit))
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	if (Has(resDesc.Desc.mImageUsage, RHIImageUsage::DepthStencilBit))	
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


	res->SetInitialState(ResourceState::kUndefined);
	return res;
}

RHIResourcePtr DX12Device::CreateBufferExt(const RHIBufferDesc& bufferDesc)
{

	RHIResourcePtr res = CreateRHIObject<DX12ResourceNew>();

	res->mResType = ResourceType::kBuffer;
	res->mResSize = bufferDesc.mSize;	
	res->mDimension = RHIResDimension::Buffer;
	DX12ResourceNew* dxRes = res->As<DX12ResourceNew>();
	auto& desc = dxRes->mDxDesc;

	desc = CD3DX12_RESOURCE_DESC::Buffer(bufferDesc.mSize);	
	ResourceState state = ResourceState::kCommon;
	return res;
}

}
