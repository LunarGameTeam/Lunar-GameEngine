#pragma once
#include "LDirectXCommon.h"
namespace luna
{
	struct RootSignatureParameterDesc
	{
		D3D12_DESCRIPTOR_RANGE_TYPE range_type = D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		LunarObjectID num_descriptors;
		LunarObjectID base_shader_register;
		LunarObjectID register_space;
		D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAGS::D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
		LunarObjectID num_descriptor_ranges;
		D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
	};
	INIT_REFLECTION_CLASS(RootSignatureParameterDesc,
		reflect_data.range_type,
		reflect_data.num_descriptors,
		reflect_data.base_shader_register,
		reflect_data.register_space,
		reflect_data.flags,
		reflect_data.num_descriptor_ranges,
		reflect_data.shader_visibility
	);
	INIT_REFLECTION_CLASS(D3D12_STATIC_SAMPLER_DESC,
		reflect_data.Filter,
		reflect_data.AddressU,
		reflect_data.AddressV,
		reflect_data.AddressW,
		reflect_data.MipLODBias,
		reflect_data.MaxAnisotropy,
		reflect_data.ComparisonFunc,
		reflect_data.BorderColor,
		reflect_data.MinLOD,
		reflect_data.MaxLOD,
		reflect_data.ShaderRegister,
		reflect_data.RegisterSpace,
		reflect_data.ShaderVisibility
	);
	struct RootSignatureDesc
	{
		LVector<RootSignatureParameterDesc> root_parameter_data;
		LVector<D3D12_STATIC_SAMPLER_DESC> static_sampler_data;
		D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE;
	};
	INIT_REFLECTION_CLASS(RootSignatureDesc,
		reflect_data.root_parameter_data,
		reflect_data.static_sampler_data,
		reflect_data.root_signature_flags
	);
	class LDx12GraphicResourceRootSignature :public LTemplateAsset<RootSignatureDesc>
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_root_signature_data;
	public:
		LDefaultAssert(LDx12GraphicResourceRootSignature, RootSignatureDesc)
			inline ID3D12RootSignature* GetRootSignature()
		{
			return m_root_signature_data.Get();
		}
	private:
		luna::LResult BuildRootSignatureResource(
			LString root_signature_name,
			const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& rootSignatureDesc,
			Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature_data,
			ID3D12Device* directx_device
		);
		void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override;
		luna::LResult InitResorceByDesc(const RootSignatureDesc& resource_desc) override;
	};
	void InitRootSignatureJsonReflect();
}
