#include "Graphics/RHI/DirectX12/DX12View.h"

#include "Graphics/RHI/DirectX12/DX12DescriptorPool.h"
#include "Graphics/RHI/DirectX12/DX12Resource.h"
#include "Graphics/RHI//DirectX12/DX12Device.h"
namespace luna::graphics 
{

DX12View::DX12View(const ViewDesc& viewDesc) :
RHIView(viewDesc)
{
	DX12Device* device = sGlobelRenderDevice->As<DX12Device>();
	GetDescriptorType(mHeapType);
	mUsedHeap = device->GetCpuDescriptorHeapByType(mHeapType);
	mViewIndex = mUsedHeap->AllocView();
	mCPUHandle = mUsedHeap->GetCpuHandle(mViewIndex);
}

DX12View::~DX12View()
{
	DX12Device* device = sGlobelRenderDevice->As<DX12Device>();
	device->GetCpuDescriptorHeapByType(mHeapType)->ReleaseView(mViewIndex);
}
void DX12View::GetDimensionSrv(const RHIResDimension resDimension,D3D12_SHADER_RESOURCE_VIEW_DESC &srvDesc)
{
	switch (resDimension)
	{
	case RHIResDimension::Buffer:
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = mBindResource->GetDesc().Width / mViewDesc.mStructureStride;
		srvDesc.Buffer.StructureByteStride = mViewDesc.mStructureStride;
		break;
	case RHIResDimension::Texture1D:
		srvDesc.Texture1D.MipLevels = mViewDesc.mLevelCount;
		srvDesc.Texture1D.MostDetailedMip = 0;
		srvDesc.Texture1D.ResourceMinLODClamp = 0;
		break;
	case RHIResDimension::Texture2D:
	{
		srvDesc.Texture2D.MipLevels = mViewDesc.mLevelCount;
		if (mViewDesc.mViewDimension == RHIViewDimension::TextureView2DArray)
		{
			srvDesc.Texture2DArray.ArraySize = mViewDesc.mLevelCount;
		}
		break;
	}

	case RHIResDimension::Texture3D:

	default:
		break;
	}
}

void DX12View::GetDimensionUav(const RHIResDimension resDimension, D3D12_UNORDERED_ACCESS_VIEW_DESC& uavDesc)
{
	switch (resDimension)
	{
	case RHIResDimension::Buffer:
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAGS::D3D12_BUFFER_UAV_FLAG_NONE;
		uavDesc.Buffer.NumElements = mBindResource->GetDesc().Width / mViewDesc.mStructureStride;
		uavDesc.Buffer.StructureByteStride = mViewDesc.mStructureStride;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		break;
	case RHIResDimension::Texture1D:
		uavDesc.Texture1D.MipSlice = 0;
		break;
	case RHIResDimension::Texture2D:
	{
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.Texture2D.PlaneSlice = 0;
		if (mViewDesc.mViewDimension == RHIViewDimension::TextureView2DArray)
		{
			uavDesc.Texture2DArray.ArraySize = mViewDesc.mLevelCount;
		}
		break;
	}

	case RHIResDimension::Texture3D:

	default:
		break;
	}
}

void DX12View::BindResource(RHIResource* buffer_data)
{
	mBindResource = buffer_data;
	ID3D12Device* device = sGlobelRenderDevice->As<DX12Device>()->GetDx12Device();
	DX12Resource* dx12Res = mBindResource->As<DX12Resource>();
	if (dx12Res->mDxRes == nullptr)
	{
		return;
	}
	switch (mViewType)
	{
	case RHIViewType::kConstantBuffer:
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferDesc = {};
		constantBufferDesc.BufferLocation = dx12Res->mDxRes->GetGPUVirtualAddress() + mViewDesc.mOffset;
		constantBufferDesc.SizeInBytes = mViewDesc.mBufferSize;
		device->CreateConstantBufferView(&constantBufferDesc, mCPUHandle);
		break;
	}
	case RHIViewType::kStructuredBuffer:
	case RHIViewType::kTexture:
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = GetGraphicFormat(dx12Res->GetDesc().Format);
		srvDesc.ViewDimension = GetSrvDimentionByView(mViewDesc.mViewDimension);
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		GetDimensionSrv(dx12Res->GetDesc().Dimension, srvDesc);
		device->CreateShaderResourceView(dx12Res->mDxRes.Get(), &srvDesc, mCPUHandle);

		break;
	}
	case RHIViewType::kRenderTarget:
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = GetGraphicFormat(dx12Res->GetDesc().Format);
		rtvDesc.ViewDimension = GetRtvDimention(mViewDesc.mViewDimension);

		device->CreateRenderTargetView(dx12Res->mDxRes.Get(), &rtvDesc, mCPUHandle);
		break;
	}
	case RHIViewType::kDepthStencil:
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = GetGraphicFormat(dx12Res->GetDesc().Format);
		dsvDesc.ViewDimension = GetDsvDimention(mViewDesc.mViewDimension);
		device->CreateDepthStencilView(dx12Res->mDxRes.Get(), &dsvDesc, mCPUHandle);
		break;
	}
	case RHIViewType::kSampler:
	{
		device->CreateSampler(&(dx12Res->mDxSamplerDesc), mCPUHandle);
		break;
	}
	case RHIViewType::kRWStructuredBuffer:
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = GetGraphicFormat(dx12Res->GetDesc().Format);
		uavDesc.ViewDimension = GetUavDimentionByView(mViewDesc.mViewDimension);
		GetDimensionUav(dx12Res->GetDesc().Dimension, uavDesc);
		device->CreateUnorderedAccessView(dx12Res->mDxRes.Get(),nullptr, &uavDesc, mCPUHandle);
		break;
	}
	default:
		assert(0);
	}
}

bool DX12View::GetDescriptorType(D3D12_DESCRIPTOR_HEAP_TYPE& descriptor_type)
{
	switch (mViewType)
	{
	case RHIViewType::kConstantBuffer:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	case RHIViewType::kStructuredBuffer:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	case RHIViewType::kTexture:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	case RHIViewType::kUnorderedAccess:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	case RHIViewType::kSampler:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		break;
	}
	case RHIViewType::kRenderTarget:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		break;
	}
	case RHIViewType::kDepthStencil:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		break;
	}
	case RHIViewType::kRWStructuredBuffer:
	{
		descriptor_type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	}
	default:
	{
		assert(0);
		break;
	}
	}
	return true;
}

}