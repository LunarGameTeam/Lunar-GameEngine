#include "render/rhi/DirectX12/dx12_resource.h"
#include "render/rhi/DirectX12/dx12_memory.h"
#include "render/rhi/DirectX12/dx12_swapchain.h"
#include "render/rhi/DirectX12/dx12_device.h"
#include "render/render_module.h"

using Microsoft::WRL::ComPtr;


namespace luna::render
{
	DX12Resource::DX12Resource(const RHIBufferDesc& buffer_desc)
	{

		mResDesc.mType = ResourceType::kBuffer;
		mResDesc.Dimension = RHIResDimension::Buffer;
		mResDesc.Width = buffer_desc.mSize;

		mDxDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		mDxDesc.Alignment = 0;
		mDxDesc.Width = mResDesc.Width;
		mDxDesc.Height = 1;
		mDxDesc.DepthOrArraySize = 1;
		mDxDesc.MipLevels = 1;
		mDxDesc.Format = DXGI_FORMAT_UNKNOWN;
		mDxDesc.SampleDesc.Count = 1;
		mDxDesc.SampleDesc.Quality = 0;
		mDxDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		mDxDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		SetInitialState(ResourceState::kUndefined);

		RefreshMemoryRequirements();
	}

	DX12Resource::DX12Resource(const SamplerDesc& desc)
	{
		mResDesc.Dimension = RHIResDimension::Unknown;
		mResDesc.mType = ResourceType::kSampler;

		switch (desc.filter)
		{
		case SamplerFilter::kAnisotropic:
			mDxSamplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
			break;
		case SamplerFilter::kMinMagMipLinear:
			mDxSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case SamplerFilter::kComparisonMinMagMipLinear:
			mDxSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			break;
		}

		switch (desc.mode)
		{
		case SamplerTextureAddressMode::kWrap:
			mDxSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			mDxSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			mDxSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			break;
		case SamplerTextureAddressMode::kClamp:
			mDxSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			mDxSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			mDxSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			break;
		}

		switch (desc.func)
		{
		case SamplerComparisonFunc::kNever:
			mDxSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			break;
		case SamplerComparisonFunc::kAlways:
			mDxSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			break;
		case SamplerComparisonFunc::kLess:
			mDxSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
			break;
		}

		mDxSamplerDesc.MinLOD = 0;
		mDxSamplerDesc.MaxLOD = std::numeric_limits<float>::max();
		mDxSamplerDesc.MaxAnisotropy = 1;
	}

	DX12Resource::DX12Resource(const RHITextureDesc& textureDesc, const RHIResDesc& resDesc) : RHIResource(resDesc)
	{
		mDxDesc.Width = resDesc.Width;
		mDxDesc.Height = resDesc.Height;
		mDxDesc.DepthOrArraySize = resDesc.DepthOrArraySize;
		mDxDesc.MipLevels = resDesc.MipLevels;

		switch (resDesc.Format)
		{
		case RHITextureFormat::FORMAT_R8G8BB8A8_UNORM:
			mDxDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case RHITextureFormat::FORMAT_D24_UNORM_S8_UINT:
			mDxDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			mDxDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		}
		switch (resDesc.Dimension)
		{
		case RHIResDimension::Texture1D:
			mDxDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
			break;
		case RHIResDimension::Texture2D:
			mDxDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			break;
		case RHIResDimension::Texture3D:
			mDxDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			break;
		}

		mDxDesc.SampleDesc.Count = resDesc.SampleDesc.Count;
		mDxDesc.SampleDesc.Quality = 0;

		if (Has(resDesc.mImageUsage, RHIImageUsage::ColorAttachmentBit))
			mDxDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		if (Has(resDesc.mImageUsage, RHIImageUsage::DepthStencilBit))
			mDxDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		SetInitialState(ResourceState::kUndefined);

		RefreshMemoryRequirements();
	}

	DX12Resource::DX12Resource(uint32_t backBufferId, DX12SwapChain* swapchain)
	{
		swapchain->mSwapChain->GetBuffer(backBufferId, IID_PPV_ARGS(&mDxRes));
		mDxDesc = mDxRes->GetDesc();

		mResDesc.mType = ResourceType::kTexture;
		mResDesc.Format = GetGraphicFormatFromDx(mDxDesc.Format);
		mResDesc.Dimension = GetResourceDimensionDx(mDxDesc.Dimension);
		mResDesc.Width = mDxDesc.Width;
		mResDesc.Height = mDxDesc.Height;
		mResDesc.mImageUsage = RHIImageUsage::ColorAttachmentBit;
		m_initial_state = ResourceState::kPresent;

		RefreshMemoryRequirements();
	}

	void DX12Resource::BindMemory(RHIMemory* memory, uint64_t offset)
	{
		ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
		mBindMemory = memory;

		// TODO
		if (mBindMemory->mMemoryDesc.Type == RHIHeapType::Upload)
			SetInitialState(ResourceState::kGenericRead);
		else
			SetInitialState(ResourceState::kCommon);

		DX12Memory* dx_memory = mBindMemory->As<DX12Memory>();
		device->CreatePlacedResource(
			dx_memory->mHeap.Get(),
			offset,
			&mDxDesc,
			DxConvertState(GetInitialState()),
			nullptr,
			IID_PPV_ARGS(&mDxRes));
		if (dx_memory->mHeap->GetDesc().Properties.Type == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
		{
			CD3DX12_RANGE readRange(0, 0);
			HRESULT hr = mDxRes->Map(0, &readRange, reinterpret_cast<void**>(&mMapPointer));
			if (FAILED(hr))
			{
				LogError("Device", "map dynamic buffer to cpu error");
			}
		}
		else
		{
			mMapPointer = NULL;
		}


	}

	void DX12Resource::RefreshMemoryRequirements()
	{
		ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
		D3D12_RESOURCE_ALLOCATION_INFO allocation_info = device->GetResourceAllocationInfo(0, 1, &mDxDesc);
		mMemoryLayout.size = allocation_info.SizeInBytes;
		mMemoryLayout.alignment = allocation_info.Alignment;

		mLayout.pLayouts.resize(mDxDesc.DepthOrArraySize);
		mLayout.pNumRows.resize(mDxDesc.DepthOrArraySize);
		mLayout.pRowSizeInBytes.resize(mDxDesc.DepthOrArraySize);
		device->GetCopyableFootprints(
			&mDxDesc,
			0,
			mDxDesc.DepthOrArraySize,
			0,
			mLayout.pLayouts.data(),
			mLayout.pNumRows.data(),
			mLayout.pRowSizeInBytes.data(),
			&mLayout.pTotalBytes
		);
		if (mDxDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		{
			mDxDesc.Width = mMemoryLayout.size;
		}
	};

}
