#pragma once
#include "render/render_config.h"

#include "render/rhi/rhi_binding_set_layout.h"

#include "render/rhi/DirectX12/dx12_rhi.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"

#include "render/rhi/DirectX12/dx12_descriptor_pool.h"

namespace luna::render
{

class DX12BindingSetLayout;

class DX12BindingSet : public RHIBindingSet
{
public:
	TRHIPtr<DX12DescriptorPool> mPool;
	DX12BindingSet(RHIDescriptorPool* pool, RHIBindingSetLayoutPtr layout);

	void WriteBindings(const std::vector<BindingDesc>& bindings) override;

	TRHIPtr<DX12BindingSetLayout> m_layout;
	std::vector<Dx12DescriptorSet> mDescriptorSets;
private:
};

}