#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/DirectX12/DX12Pch.h"
#include"Graphics/RHI/DirectX12/DX12DescriptorImpl.h"

namespace luna::graphics
{


class RENDER_API DX12View : public RHIView
{
	size_t mViewIndex;

	D3D12_DESCRIPTOR_HEAP_TYPE mHeapType;

	D3D12_CPU_DESCRIPTOR_HANDLE mCPUHandle;

	Dx12CpuDescriptorHeap* mUsedHeap;
public:
	DX12View(const ViewDesc& viewDesc);

	~DX12View();

	void BindResource(RHIResource* buffer_data) override;

	Dx12CpuDescriptorHeap* GetHeap() { return mUsedHeap; }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuHandle() { return mCPUHandle; };

	const D3D12_DESCRIPTOR_HEAP_TYPE& GetDescriptorHeapType() { return mHeapType; }
private:
	bool GetDescriptorType(D3D12_DESCRIPTOR_HEAP_TYPE& descriptor_type);
};
}