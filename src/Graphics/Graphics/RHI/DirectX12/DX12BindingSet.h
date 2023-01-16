#pragma once
#include "Graphics/RenderConfig.h"

#include "Graphics/RHI/RHIBindingSetLayout.h"

#include "Graphics/RHI/DirectX12/DX12Pch.h"
#include "Graphics/RHI/DirectX12/DX12DescriptorImpl.h"

#include "Graphics/RHI/DirectX12/DX12DescriptorPool.h"

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