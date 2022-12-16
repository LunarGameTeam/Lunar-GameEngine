#pragma once
#include "render/render_config.h"
#include "render/rhi/DirectX12/dx12_rhi.h"
#include "render/rhi/rhi_binding_set_layout.h"


namespace luna::render
{

struct BindingLayout
{
	D3D12_DESCRIPTOR_HEAP_TYPE mHeapType;
	size_t mInTableOffset;
};

struct DescriptorTableDesc
{
	D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
	size_t heap_offset;
	bool bindless;
	bool is_compute;
};

struct DX12DescriptorSetLayoutMember
{
	uint32_t root_table_indedx_input = -1;
	LVector<D3D12_DESCRIPTOR_RANGE> each_range_input;
	uint32_t mShaderInputNum = 0;
	LUnorderedMap<LString, int32_t> range_name_list_input;

	uint32_t root_table_indedx_sampler = -1;
	LVector<D3D12_DESCRIPTOR_RANGE> each_range_sampler;
	uint32_t mSamplerNum = 0;
	LUnorderedMap<LString, int32_t> range_name_list_sampler;
};

class RENDER_API DX12BindingSetLayout : public RHIBindingSetLayout
{
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature_data;
public:
	DX12BindingSetLayout(const std::vector<RHIBindPoint>& bindKeys);

	inline ID3D12RootSignature* GetRootSignature()
	{
		return mRootsignature.Get();
	}

	const LUnorderedMap<uint32_t, DX12DescriptorSetLayoutMember>& GetLayout() const { return mDescriptorSetLayout; }

private:
	LUnorderedMap<uint32_t, DX12DescriptorSetLayoutMember> mDescriptorSetLayout;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootsignature;

};
}
