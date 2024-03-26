#include "Core/CoreMin.h"

#include "Graphics/RHI/DirectX12/DX12BindingSet.h"
#include "Graphics/RHI/DirectX12/DX12BindingSetLayout.h"
#include "Graphics/RHI/DirectX12/DX12DescriptorImpl.h"
#include "DX12View.h"




namespace luna::graphics
{

void DX12BindingSet::WriteBindings( const std::vector<BindingDesc>& bindings)
{
	
	for (const BindingDesc& binding : bindings)
	{
		if (!binding.mView)
		{
			continue;
		}
		const RHIBindPoint& bindPoint = binding.mBindTarget;
		DX12View* dx12Res = binding.mView->As<DX12View>();
		auto layout = m_layout->As<DX12BindingSetLayout>()->GetLayout();
		DX12DescriptorSetLayoutMember& layout_set = layout.find(bindPoint.mSpace)->second;


		Dx12DescriptorSet& now_descriptor_set = mDescriptorSets[bindPoint.mSpace];
		ID3D12Device* device = sGlobelRenderDevice->As<DX12Device>()->GetDx12Device();
		const D3D12_DESCRIPTOR_HEAP_TYPE& heap_type_value = dx12Res->GetDescriptorHeapType();

		int32_t index_range = -1;
		uint32_t bind_offset = -1;
		uint32_t descriptor_size = -1;
		if (heap_type_value == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			index_range = layout_set.range_name_list_input.find(bindPoint.mName)->second;
			bind_offset = layout_set.each_range_input[index_range].OffsetInDescriptorsFromTableStart;
			descriptor_size = layout_set.each_range_input[index_range].NumDescriptors;
		}
		else
		{
			index_range = layout_set.range_name_list_sampler.find(bindPoint.mName)->second;
			bind_offset = layout_set.each_range_sampler[index_range].OffsetInDescriptorsFromTableStart;
			descriptor_size = layout_set.each_range_sampler[index_range].NumDescriptors;
		}
		//暂时先把set里的资源一个一个的更新一下
		for (int32_t i = 0; i < descriptor_size; ++i)
		{
			Dx12DescriptorList& set_handle_desc = now_descriptor_set.mDescriptorLists[heap_type_value];
			D3D12_CPU_DESCRIPTOR_HANDLE m_handle = set_handle_desc.mCPUHandle;
			m_handle.ptr += set_handle_desc.mPerDescriptorSize * (bind_offset + i);

			device->CopyDescriptorsSimple(
				1,
				m_handle,
				dx12Res->GetCpuHandle(),
				heap_type_value);
		}
	}
}

DX12BindingSet::DX12BindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout) :	
	m_layout(layout)
{
	mPool = pool->As<DX12DescriptorPool>();
	DX12DescriptorPool* dx12Pool = pool->As<DX12DescriptorPool>();
	DX12BindingSetLayout* dx12Layout = layout->As<DX12BindingSetLayout>();
	const LUnorderedMap<uint32_t, DX12DescriptorSetLayoutMember>& m_layout = dx12Layout->GetLayout();
	for (uint32_t id_layout = 0; id_layout < m_layout.size(); ++id_layout)
	{
		const DX12DescriptorSetLayoutMember&m_range = m_layout.find(id_layout)->second;
		Dx12DescriptorSet& new_set = mDescriptorSets.emplace_back();
		mPool->SelectSegment(DescriptorHeapType::CBV_SRV_UAV)->AllocDescriptorSet(m_range.mShaderInputNum, new_set);
		mPool->SelectSegment(DescriptorHeapType::SAMPLER)->AllocDescriptorSet(m_range.mSamplerNum, new_set);
	}
	//TODO
}

}