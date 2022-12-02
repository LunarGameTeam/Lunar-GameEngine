#pragma once

#include "dx12_rhi.h"

#include "render/rhi/rhi_descriptor_heap.h"

#include "render/rhi/DirectX12/dx12_device.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"

#include "render/render_module.h"


namespace luna::render
{

class DX12TypedDescriptorHeap;


class RENDER_API DX12DescriptorPool : public RHIDescriptorPool
{
public:
	DX12DescriptorPool(const DescriptorPoolDesc& poolDesc);
	DX12GpuDescriptorSegment* SelectSegment(DescriptorHeapType view_type);
private:
	TRHIPtr<DX12GpuDescriptorSegment> mSrvSegment;
	TRHIPtr<DX12GpuDescriptorSegment> mSamplerSegment;

	bool Resize(const DescriptorPoolDesc& resource_desc);
};


}
