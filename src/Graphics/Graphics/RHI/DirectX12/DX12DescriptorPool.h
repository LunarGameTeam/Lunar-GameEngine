#pragma once

#include "DX12Pch.h"

#include "Graphics/RHI/RHIDescriptorHeap.h"

#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12DescriptorImpl.h"

#include "Graphics/RenderModule.h"


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
