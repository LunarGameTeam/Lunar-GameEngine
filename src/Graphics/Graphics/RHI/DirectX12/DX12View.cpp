#include "Graphics/RHI/DirectX12/DX12View.h"

#include "Graphics/RHI/DirectX12/DX12DescriptorPool.h"
#include "Graphics/RHI/DirectX12/DX12Resource.h"
#include "Graphics/RHI//DirectX12/DX12Device.h"

#include "Graphics/RenderModule.h"

namespace luna::render 
{

DX12View::DX12View(const ViewDesc& viewDesc) :
RHIView(viewDesc)
{
	viewDesc.mViewType;
	DX12Device* device = sRenderModule->GetDevice<DX12Device>();
	GetDescriptorType(mHeapType);
	mUsedHeap = device->GetCpuDescriptorHeapByType(mHeapType);
	mViewIndex = mUsedHeap->AllocView();
	mCPUHandle = mUsedHeap->GetCpuHandle(mViewIndex);
}

DX12View::~DX12View()
{
	DX12Device* device = sRenderModule->GetDevice<DX12Device>();
	device->GetCpuDescriptorHeapByType(mHeapType)->ReleaseView(mViewIndex);
}

void DX12View::BindResource(RHIResource* buffer_data)
{
	mBindResource = buffer_data;
	ID3D12Device* device = sRenderModule->GetDevice<DX12Device>()->GetDx12Device();
	DX12Resource* dx12Res = mBindResource->As<DX12Resource>();
	switch (mViewType)
	{
	case RHIViewType::kConstantBuffer:
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC constantBufferDesc = {};
		constantBufferDesc.BufferLocation = dx12Res->mDxRes->GetGPUVirtualAddress();
		constantBufferDesc.SizeInBytes = Alignment(dx12Res->GetMemoryRequirements().size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		device->CreateConstantBufferView(&constantBufferDesc, mCPUHandle);
		break;
	}
	case RHIViewType::kTexture:
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = GetGraphicFormat(dx12Res->GetDesc().Format);
		srvDesc.ViewDimension = GetSrvDimentionByView(mViewDesc.mViewDimension);
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		switch (dx12Res->GetDesc().Dimension)
		{
		case RHIResDimension::Texture1D:
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAGS::D3D12_BUFFER_SRV_FLAG_NONE;
			srvDesc.Buffer.NumElements = 1;
			srvDesc.Buffer.StructureByteStride = mBindResource->GetMemoryRequirements().size;
			break;
		case RHIResDimension::Texture2D:
			srvDesc.Texture2D.MipLevels = mViewDesc.mLevelCount;
			break;
		case RHIResDimension::Texture3D:

		default:
			break;
		}
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
	default:
	{
		assert(0);
		break;
	}
	}
	return true;
}

}