#include "Graphics/RHI/DirectX12/DX12BindingSetLayout.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"

#include "Graphics/RenderModule.h"

using Microsoft::WRL::ComPtr;


namespace luna::graphics
{


D3D12_SHADER_VISIBILITY GetVisibility(RHIShaderType shader_type)
{
	D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;
	switch (shader_type)
	{
	case RHIShaderType::Vertex:
		visibility = D3D12_SHADER_VISIBILITY_VERTEX;
		break;
	case RHIShaderType::Pixel:
		visibility = D3D12_SHADER_VISIBILITY_PIXEL;
		break;
	case RHIShaderType::Compute:
		visibility = D3D12_SHADER_VISIBILITY_ALL;
		break;
	}
	return visibility;
}

D3D12_DESCRIPTOR_RANGE_TYPE GetRangeType(RHIViewType view_type)
{
	D3D12_DESCRIPTOR_RANGE_TYPE range_type;
	switch (view_type)
	{
	case RHIViewType::kTexture:
	case RHIViewType::kBuffer:
	case RHIViewType::kStructuredBuffer:
		return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	case RHIViewType::kRWTexture:
	case RHIViewType::kRWBuffer:
	case RHIViewType::kRWStructuredBuffer:
		return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	case RHIViewType::kConstantBuffer:
		return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	case RHIViewType::kSampler:
		return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	default:
		throw std::runtime_error("wrong view type");
	}
}

D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType(RHIViewType view_type)
{
	D3D12_DESCRIPTOR_RANGE_TYPE range_type;
	switch (GetRangeType(view_type))
	{
	case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
	case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
	case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
		return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	default:
		throw std::runtime_error("wrong view type");
	}
}

DX12BindingSetLayout::DX12BindingSetLayout(const std::vector<RHIBindPoint>& descs)
{
	for (const RHIBindPoint& bind_key : descs)
	{
		DX12DescriptorSetLayoutMember& now_descriptor_set = mDescriptorSetLayout[bind_key.mSpace];
		auto now_range_type =  GetRangeType(bind_key.mViewType);
		D3D12_DESCRIPTOR_RANGE* now_range = nullptr;
		switch (now_range_type)
		{
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
		case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
			now_range = &now_descriptor_set.each_range_input.emplace_back();
			now_range->OffsetInDescriptorsFromTableStart = now_descriptor_set.mShaderInputNum;
			now_descriptor_set.mShaderInputNum += bind_key.mCount;
			now_descriptor_set.range_name_list_input[bind_key.mName] = now_descriptor_set.each_range_input.size() - 1;
			break;
		case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
			now_range = &now_descriptor_set.each_range_sampler.emplace_back();
			now_range->OffsetInDescriptorsFromTableStart = now_descriptor_set.mSamplerNum;
			now_descriptor_set.mSamplerNum += bind_key.mCount;
			now_descriptor_set.range_name_list_sampler[bind_key.mName] = now_descriptor_set.each_range_sampler.size() - 1;
			break;
		default:
			assert(false);
		}
		now_range->RangeType = now_range_type;
		now_range->NumDescriptors = bind_key.mCount;
		now_range->BaseShaderRegister = bind_key.mSlot;
		now_range->RegisterSpace = bind_key.mSpace;
	}
	std::vector<D3D12_ROOT_PARAMETER> rootParams;
	for (int32_t descriptor_set_id = 0; descriptor_set_id < mDescriptorSetLayout.size(); ++descriptor_set_id)
	{
		DX12DescriptorSetLayoutMember& now_descriptor_set = mDescriptorSetLayout[descriptor_set_id];
		if(now_descriptor_set.each_range_input.size() > 0)
		{
			now_descriptor_set.root_table_indedx_input = rootParams.size();
			D3D12_ROOT_DESCRIPTOR_TABLE decriptorTable = {};
			decriptorTable.NumDescriptorRanges = (UINT)now_descriptor_set.each_range_input.size();
			decriptorTable.pDescriptorRanges = now_descriptor_set.each_range_input.data();

			D3D12_ROOT_PARAMETER& rootParam = rootParams.emplace_back();
			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParam.DescriptorTable = decriptorTable;
			rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		}
		if (now_descriptor_set.each_range_sampler.size() > 0)
		{
			now_descriptor_set.root_table_indedx_sampler = rootParams.size();
			D3D12_ROOT_DESCRIPTOR_TABLE decriptorTable = {};
			decriptorTable.NumDescriptorRanges = (UINT)now_descriptor_set.each_range_sampler.size();
			decriptorTable.pDescriptorRanges = now_descriptor_set.each_range_sampler.data();

			D3D12_ROOT_PARAMETER& rootParam = rootParams.emplace_back();
			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParam.DescriptorTable = decriptorTable;
			rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		}
	}

	D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_SIGNATURE_DESC rootsignatureDesc = {};
	rootsignatureDesc.Init(static_cast<uint32_t>(rootParams.size()),
		rootParams.data(),
		0,
		nullptr,
		root_signature_flags
	);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> errorBlob;
	assert(SUCCEEDED(D3D12SerializeRootSignature(&rootsignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBlob),
		"%s", static_cast<char*>(errorBlob->GetBufferPointer())
	));
	HRESULT hr = sRenderModule->GetDevice<DX12Device>()->GetDx12Device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootsignature));	
	assert(SUCCEEDED(hr));
}


}
