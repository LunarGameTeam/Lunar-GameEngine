#pragma once
#include "LDirectXCommon.h"
INIT_REFLECTION_CLASS(D3D12_HEAP_DESC,
	reflect_data.Alignment,
	reflect_data.Flags,
	reflect_data.Properties.CPUPageProperty,
	reflect_data.Properties.CreationNodeMask,
	reflect_data.Properties.MemoryPoolPreference,
	reflect_data.Properties.Type,
	reflect_data.Properties.VisibleNodeMask,
	reflect_data.SizeInBytes
);
namespace luna
{
	class LDx12GraphicResourceHeap :public LTemplateAsset<D3D12_HEAP_DESC>
	{
		Microsoft::WRL::ComPtr<ID3D12Heap> resource_heap_data;
	public:
		LDefaultAssert(LDx12GraphicResourceHeap, D3D12_HEAP_DESC)
			inline ID3D12Heap* GetResourceHeap()
		{
			if (resource_heap_data != nullptr)
			{
				return resource_heap_data.Get();
			}
			return nullptr;
		}
	private:
		void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override;
		luna::LResult InitResorceByDesc(const D3D12_HEAP_DESC& resource_desc) override;
	};
}