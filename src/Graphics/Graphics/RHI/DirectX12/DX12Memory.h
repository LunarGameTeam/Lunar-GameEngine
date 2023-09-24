#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIMemory.h"
#include "DX12Pch.h"
#include "D3D12MemAlloc.h"

namespace luna::graphics
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
