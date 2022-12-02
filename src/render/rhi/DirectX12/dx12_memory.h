#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_memory.h"
#include "dx12_rhi.h"


namespace luna::render
{
class RENDER_API DX12Memory : public RHIMemory
{
public:
	Microsoft::WRL::ComPtr<ID3D12Heap> mHeap;

	DX12Memory(const RHIMemoryDesc& resource_desc);

	inline ID3D12Heap* GetResourceHeap() const
	{
		if (mHeap != nullptr)
		{
			return mHeap.Get();
		}
		return nullptr;
	}
};
}
