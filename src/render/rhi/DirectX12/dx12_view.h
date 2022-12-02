#pragma once

#include "render/pch.h"
#include "render/rhi/DirectX12/dx12_rhi.h"
#include"render/rhi/DirectX12/dx12_descriptor_impl.h"

namespace luna::render
{


class RENDER_API DX12View : public RHIView
{
	size_t mViewIndex;

	D3D12_DESCRIPTOR_HEAP_TYPE mHeapType;

	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHandle;

	RHIResourcePtr mBindRes;

	Dx12CpuDescriptorHeap* mUsedHeap;
public:
	DX12View(const ViewDesc& viewDesc);

	~DX12View();

	void BindResource(RHIResource* buffer_data) override;

	RHIResourcePtr& GetRessource() { return mBindRes; };

	Dx12CpuDescriptorHeap* GetHeap() { return mUsedHeap; }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return mCPUHandle; };

	const D3D12_DESCRIPTOR_HEAP_TYPE& GetDescriptorHeapType() { return mHeapType; }
private:
	bool GetDescriptorType(D3D12_DESCRIPTOR_HEAP_TYPE& descriptor_type);
};
}