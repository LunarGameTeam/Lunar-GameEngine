#include "DX12DescriptorPool.h"

#include "Graphics/RenderModule.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12DescriptorImpl.h"

namespace luna::render
{

DX12DescriptorPool::DX12DescriptorPool(const DescriptorPoolDesc& poolDesc)
{
	DX12Device* m_device = sRenderModule->GetDevice<DX12Device>();

	auto srv_pool_size = poolDesc.mPoolAllocateSizes.find(DescriptorHeapType::CBV_SRV_UAV);
	if (srv_pool_size->second != 0)
	{
		mSrvSegment = m_device->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->AllocPool(srv_pool_size->second);
	}
	auto sampler_pool_size = poolDesc.mPoolAllocateSizes.find(DescriptorHeapType::CBV_SRV_UAV);
	if (sampler_pool_size->second != 0)
	{
		mSamplerSegment = m_device->GetGpuDescriptorHeapByType(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)->AllocPool(sampler_pool_size->second);
	}
}

bool DX12DescriptorPool::Resize(const DescriptorPoolDesc& heapDesc)
{
	assert(false);
	return true;
};

DX12GpuDescriptorSegment* DX12DescriptorPool::SelectSegment(DescriptorHeapType view_type)
{

	switch (view_type)
	{
	case DescriptorHeapType::CBV_SRV_UAV:
		return mSrvSegment.get();
		break;
	case DescriptorHeapType::SAMPLER:
		return mSamplerSegment.get();
		break;
	case DescriptorHeapType::NUM_TYPES:
		break;
	default:
		break;
	}
	assert(false);
	return mSrvSegment.get();
}

}
