#pragma once
#include "LDirectXCommon.h"
INIT_REFLECTION_CLASS(D3D12_DESCRIPTOR_HEAP_DESC,
	reflect_data.Type,
	reflect_data.NumDescriptors,
	reflect_data.Flags,
	reflect_data.NodeMask
);
namespace luna
{
	class LDx12GraphicDescriptorHeap :public LTemplateAsset<D3D12_DESCRIPTOR_HEAP_DESC>
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor_heap_data;
	public:
		LDefaultAssert(LDx12GraphicDescriptorHeap, D3D12_DESCRIPTOR_HEAP_DESC)
		inline ID3D12DescriptorHeap* GetResourceHeap()
		{
			if (descriptor_heap_data != nullptr)
			{
				return descriptor_heap_data.Get();
			}
			return nullptr;
		}
	private:
		void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override;
		luna::LResult InitResorceByDesc(const D3D12_DESCRIPTOR_HEAP_DESC& resource_desc) override;
	};
	void InitDescriptorHeapJsonReflect();
}