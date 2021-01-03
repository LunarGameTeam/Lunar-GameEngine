#include "LDirectX12RootSignature.h"
using namespace luna;
using Microsoft::WRL::ComPtr;
luna::LResult LDx12GraphicResourceRootSignature::BuildRootSignatureResource(
	LString root_signature_name,
	const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& rootSignatureDesc,
	ComPtr<ID3D12RootSignature> root_signature_data,
	ID3D12Device* directx_device
)
{
	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	HRESULT hr;
	hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "serial rootsignature " + root_signature_name + " error", error_message);

		return error_message;
	}
	hr = directx_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature_data));
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "Create root signature " + root_signature_name + " failed", error_message);

		return error_message;
	}
	return g_Succeed;
}
void LDx12GraphicResourceRootSignature::CheckIfLoadingStateChanged(LLoadState& m_object_load_state)
{
	m_object_load_state = LLoadState::LOAD_STATE_FINISHED;
};
luna::LResult LDx12GraphicResourceRootSignature::InitResorceByDesc(const RootSignatureDesc& resource_desc)
{
	ID3D12Device* directx_device = reinterpret_cast<ID3D12Device*>(ILunarGraphicDeviceCore::GetInstance()->GetVirtualDevice());
	if (directx_device == nullptr)
	{
		LResult error_message;
		LunarDebugLogError(0, "Directx device is broken ", error_message);
		return error_message;
	}

	LResult check_error;
	//创建rootsignature
	D3D12_STATIC_SAMPLER_DESC* data_sampledesc;
	CD3DX12_DESCRIPTOR_RANGE1* ranges;
	CD3DX12_ROOT_PARAMETER1* rootParameters;
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc_out;
	auto num_parameter = resource_desc.root_parameter_data.size();
	if (num_parameter > 0)
	{
		ranges = new CD3DX12_DESCRIPTOR_RANGE1[num_parameter];
		rootParameters = new CD3DX12_ROOT_PARAMETER1[num_parameter];
	}
	else
	{
		ranges = NULL;
		rootParameters = NULL;
	}
	int32_t all_descriptor_num = 0;
	for (int i = 0; i < num_parameter; ++i)
	{
		//获取需要range_type(cbv/srv/......)
		D3D12_DESCRIPTOR_RANGE_TYPE range_type = resource_desc.root_parameter_data[i].range_type;
		//获取需要创建的descriptor(cbv/srv/......)的数量
		int32_t descriptor_num = resource_desc.root_parameter_data[i].num_descriptors;
		//获取这组descriptor变量对应的首个寄存器的地址
		int32_t base_registor = resource_desc.root_parameter_data[i].base_shader_register;
		//获取这组descriptor变量对应的寄存器域的地址
		int32_t base_registor_space = resource_desc.root_parameter_data[i].register_space;
		//获取这组descriptor变量对应的格式
		D3D12_DESCRIPTOR_RANGE_FLAGS flag_type = resource_desc.root_parameter_data[i].flags;
		//注册descriptor range
		ranges[i].Init(range_type, descriptor_num, base_registor, base_registor_space, flag_type);
		//获取该descriptor range需要被创建的次数
		int32_t descriptor_range_num = resource_desc.root_parameter_data[i].num_descriptor_ranges;
		//获取shader的访问权限
		D3D12_SHADER_VISIBILITY shader_visibility_type = resource_desc.root_parameter_data[i].shader_visibility;
		//注册rootsignature格式
		rootParameters[i].InitAsDescriptorTable(descriptor_range_num, &ranges[i], shader_visibility_type);
		all_descriptor_num += descriptor_num;
	}
	LunarObjectID num_static_sampler;
	//获取每个静态采样器
	num_static_sampler = static_cast<LunarObjectID>(resource_desc.static_sampler_data.size());
	if (num_static_sampler > 0)
	{
		data_sampledesc = new D3D12_STATIC_SAMPLER_DESC[num_static_sampler];
	}
	else
	{
		data_sampledesc = NULL;
	}
	for (LunarObjectID i = 0; i < num_static_sampler; ++i)
	{
		data_sampledesc[i] = resource_desc.static_sampler_data[i];
	}
	//获取rootsignature格式
	D3D12_ROOT_SIGNATURE_FLAGS root_signature = resource_desc.root_signature_flags;
	desc_out.Init_1_1(static_cast<UINT>(num_parameter), rootParameters, num_static_sampler, data_sampledesc, root_signature);
	//创建rootsignature
	check_error = BuildRootSignatureResource(GetObjectName().c_str(), desc_out, m_root_signature_data, directx_device);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//删除临时资源
	if (ranges != NULL)
	{
		delete[] ranges;
		ranges = NULL;
	}
	if (rootParameters != NULL)
	{
		delete[] rootParameters;
		rootParameters = NULL;
	}
	if (data_sampledesc != NULL)
	{
		delete[] data_sampledesc;
		data_sampledesc = NULL;
	}
	return luna::g_Succeed;
}
void luna::InitRootSignatureJsonReflect()
{
	InitNewStructToReflection(RootSignatureParameterDesc);
	InitNewStructToReflection(D3D12_STATIC_SAMPLER_DESC);
	InitNewStructToReflection(RootSignatureDesc);
	//descriptor range格式
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER);
	//descriptor flag格式
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
	InitNewEnumValue(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	//shader访问权限
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_ALL);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_VERTEX);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_HULL);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_DOMAIN);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_GEOMETRY);
	InitNewEnumValue(D3D12_SHADER_VISIBILITY_PIXEL);
	//采样格式
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_ANISOTROPIC);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_COMPARISON_ANISOTROPIC);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MINIMUM_ANISOTROPIC);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR);
	InitNewEnumValue(D3D12_FILTER_MAXIMUM_ANISOTROPIC);
	//寻址格式
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_WRAP);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_MIRROR);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_BORDER);
	InitNewEnumValue(D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE);
	//比较函数格式
	InitNewEnumValue(D3D12_COMPARISON_FUNC_NEVER);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_LESS);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_LESS_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_GREATER);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_NOT_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_GREATER_EQUAL);
	InitNewEnumValue(D3D12_COMPARISON_FUNC_ALWAYS);
	//超出边界的采样颜色
	InitNewEnumValue(D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK);
	InitNewEnumValue(D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
	InitNewEnumValue(D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);
	//root signature的访问权限
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_NONE);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
	InitNewEnumValue(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT);
}