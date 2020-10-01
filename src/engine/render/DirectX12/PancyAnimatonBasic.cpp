#include"PancyAnimationBasic.h"
using namespace LunarEngine;
PancySkinAnimationControl* PancySkinAnimationControl::this_instance = NULL;
//骨骼动画缓冲区
PancySkinAnimationBuffer::PancySkinAnimationBuffer(const LunarResourceSize& animation_buffer_size_in, const LunarResourceSize& bone_buffer_size_in)
{
	//获取动画结果缓冲区以及骨骼矩阵缓冲区的大小，清零当前指针的位置
	now_used_position_animation = 0;
	animation_buffer_size = animation_buffer_size_in;
	now_used_position_bone = 0;
	bone_buffer_size = bone_buffer_size_in;
}
PancySkinAnimationBuffer::~PancySkinAnimationBuffer()
{
}
LResult PancySkinAnimationBuffer::Create()
{
	LResult check_error;
	std::string file_name = "pancy_skin_mesh_buffer";
	Json::Value root_value;
	std::string buffer_subresource_name;
	//创建存储蒙皮结果的缓冲区资源(静态缓冲区)
	PancyCommonBufferDesc animation_buffer_resource_desc;
	animation_buffer_resource_desc.buffer_type = Buffer_UnorderedAccess_static;
	animation_buffer_resource_desc.buffer_res_desc.Alignment = 0;
	animation_buffer_resource_desc.buffer_res_desc.DepthOrArraySize = 1;
	animation_buffer_resource_desc.buffer_res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	animation_buffer_resource_desc.buffer_res_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	animation_buffer_resource_desc.buffer_res_desc.Height = 1;
	animation_buffer_resource_desc.buffer_res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	animation_buffer_resource_desc.buffer_res_desc.MipLevels = 1;
	animation_buffer_resource_desc.buffer_res_desc.SampleDesc.Count = 1;
	animation_buffer_resource_desc.buffer_res_desc.SampleDesc.Quality = 0;
	animation_buffer_resource_desc.buffer_res_desc.Width = animation_buffer_size;
	check_error = BuildBufferResource(
		file_name,
		animation_buffer_resource_desc,
		buffer_animation,
		true
	);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}

	//创建存储骨骼矩阵的缓冲区资源(UAV用于ping-pong操作)
	buffer_bone.resize(2);
	for (int buffer_index = 0; buffer_index < 2; ++buffer_index)
	{
		file_name = "pancy_skin_bone_buffer";
		PancyCommonBufferDesc bone_buffer_resource_desc;
		bone_buffer_resource_desc.buffer_type = Buffer_UnorderedAccess_static;
		bone_buffer_resource_desc.buffer_res_desc.Alignment = 0;
		bone_buffer_resource_desc.buffer_res_desc.DepthOrArraySize = 1;
		bone_buffer_resource_desc.buffer_res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bone_buffer_resource_desc.buffer_res_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		bone_buffer_resource_desc.buffer_res_desc.Height = 1;
		bone_buffer_resource_desc.buffer_res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bone_buffer_resource_desc.buffer_res_desc.MipLevels = 1;
		bone_buffer_resource_desc.buffer_res_desc.SampleDesc.Count = 1;
		bone_buffer_resource_desc.buffer_res_desc.SampleDesc.Quality = 0;
		bone_buffer_resource_desc.buffer_res_desc.Width = bone_buffer_size;
		check_error = BuildBufferResource(
			file_name,
			bone_buffer_resource_desc,
			buffer_bone[buffer_index],
			true
		);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	//创建全局id记录的缓冲区资源
	buffer_globel_index.resize(2);
	for (int buffer_index = 0; buffer_index < 2; ++buffer_index)
	{
		PancyCommonBufferDesc bone_parent_resource_desc;
		bone_parent_resource_desc.buffer_type = Buffer_UnorderedAccess_static;
		bone_parent_resource_desc.buffer_res_desc.Alignment = 0;
		bone_parent_resource_desc.buffer_res_desc.DepthOrArraySize = 1;
		bone_parent_resource_desc.buffer_res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bone_parent_resource_desc.buffer_res_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		bone_parent_resource_desc.buffer_res_desc.Height = 1;
		bone_parent_resource_desc.buffer_res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bone_parent_resource_desc.buffer_res_desc.MipLevels = 1;
		bone_parent_resource_desc.buffer_res_desc.SampleDesc.Count = 1;
		bone_parent_resource_desc.buffer_res_desc.SampleDesc.Quality = 0;
		bone_parent_resource_desc.buffer_res_desc.Width = bone_buffer_size;
		check_error = BuildBufferResource(
			file_name,
			bone_parent_resource_desc,
			buffer_globel_index[buffer_index],
			true
		);
	}

	if (!check_error.m_IsOK)
	{
		return check_error;
	}

	return g_Succeed;
}
void PancySkinAnimationBuffer::ClearUsedBuffer()
{
	now_used_position_animation = 0;
	animation_block_map.clear();
	now_used_position_bone = 0;
	bone_block_map.clear();
}
//从当前缓冲区中请求一块骨骼动画数据
LResult PancySkinAnimationBuffer::BuildAnimationBlock(
	const LunarResourceSize& vertex_num,
	LunarObjectID& block_id,
	SkinAnimationBlock& new_animation_block)
{
	LunarResourceSize now_ask_size = vertex_num * sizeof(mesh_animation_data);
	new_animation_block.start_pos = now_used_position_animation;
	new_animation_block.block_size = now_ask_size;
	//判断当前的动画缓冲区是否能够开出请求的顶点存储块
	if ((now_used_position_animation + now_ask_size) >= animation_buffer_size)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "The skin mesh animation buffer is full", error_message);

		return error_message;
	}
	//更新当前的缓冲区指针位置
	now_used_position_animation = now_used_position_animation + now_ask_size;
	//将内存块导入到map中
	LunarObjectID now_ID = static_cast<LunarObjectID>(animation_block_map.size());
	animation_block_map.insert(std::pair<LunarObjectID, SkinAnimationBlock>(now_ID, new_animation_block));
	block_id = now_ID;
	return g_Succeed;
}
//从当前骨骼矩阵缓冲区中请求一块数据区
LResult PancySkinAnimationBuffer::BuildBoneBlock(
	const LunarResourceSize& matrix_num,
	LunarObjectID& block_id,
	SkinAnimationBlock& new_bone_block
)
{
	LunarResourceSize now_ask_size = matrix_num * sizeof(DirectX::XMFLOAT4X4);
	new_bone_block.start_pos = now_used_position_bone;
	new_bone_block.block_size = now_ask_size;
	//判断当前的动画缓冲区是否能够开出请求的顶点存储块
	if ((now_used_position_bone + now_ask_size) >= bone_buffer_size)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "The Bone Matrix buffer is full", error_message);

		return error_message;
	}
	//更新当前的缓冲区指针位置
	now_used_position_bone = now_used_position_bone + now_ask_size;
	//将内存块导入到map中
	LunarObjectID now_ID = static_cast<LunarObjectID>(bone_block_map.size());
	bone_block_map.insert(std::pair<LunarObjectID, SkinAnimationBlock>(now_ID, new_bone_block));
	block_id = now_ID;

	return g_Succeed;
}
//骨骼动画管理器
PancySkinAnimationControl::PancySkinAnimationControl(
	const LunarResourceSize& animation_buffer_size_in,
	const LunarResourceSize& bone_buffer_size_in
)
{
	animation_buffer_size = animation_buffer_size_in;
	bone_buffer_size = bone_buffer_size_in;
	LunarObjectID Frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	skin_naimation_buffer.resize(Frame_num);
}
PancySkinAnimationControl::~PancySkinAnimationControl()
{
	LunarObjectID Frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	for (LunarObjectID i = 0; i < Frame_num; ++i)
	{
		delete skin_naimation_buffer[i];
	}
	skin_naimation_buffer.clear();
}
LResult PancySkinAnimationControl::BuildGlobelBufferDescriptorUAV(
	const std::string& descriptor_name,
	std::vector<VirtualResourcePointer>& buffer_data,
	const LunarResourceSize& buffer_size,
	const LunarResourceSize& per_element_size
)
{
	LunarObjectID Frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	std::vector<BasicDescriptorDesc> skin_animation_descriptor_desc;
	BasicDescriptorDesc skin_animation_UAV_desc;
	skin_animation_UAV_desc.basic_descriptor_type = PancyDescriptorType::DescriptorTypeUnorderedAccessView;
	skin_animation_UAV_desc.unordered_access_view_desc = {};
	skin_animation_UAV_desc.unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.StructureByteStride = static_cast<UINT>(per_element_size);
	LunarResourceSize element_num = buffer_size / per_element_size;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(element_num);
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.FirstElement = 0;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.CounterOffsetInBytes = 0;
	for (LunarObjectID i = 0; i < Frame_num; ++i)
	{
		skin_animation_descriptor_desc.push_back(skin_animation_UAV_desc);
	}
	auto check_error = PancyDescriptorHeapControl::GetInstance()->BuildCommonGlobelDescriptor(descriptor_name, skin_animation_descriptor_desc, buffer_data, true);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancySkinAnimationControl::PancySkinAnimationControl::BuildGlobelBufferDescriptorSRV(
	const std::string& descriptor_name,
	std::vector<VirtualResourcePointer>& buffer_data,
	const LunarResourceSize& buffer_size,
	const LunarResourceSize& per_element_size
)
{
	LunarObjectID Frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	std::vector<BasicDescriptorDesc> skin_animation_descriptor_desc;
	BasicDescriptorDesc skin_animation_SRV_desc;
	skin_animation_SRV_desc.basic_descriptor_type = PancyDescriptorType::DescriptorTypeShaderResourceView;
	LunarObjectID element_num = static_cast<LunarObjectID>(buffer_size / per_element_size);
	skin_animation_SRV_desc.shader_resource_view_desc = {};
	skin_animation_SRV_desc.shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	skin_animation_SRV_desc.shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.StructureByteStride = static_cast<UINT>(per_element_size);
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(element_num);;
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.FirstElement = 0;
	for (LunarObjectID i = 0; i < Frame_num; ++i)
	{
		skin_animation_descriptor_desc.push_back(skin_animation_SRV_desc);
	}
	auto check_error = PancyDescriptorHeapControl::GetInstance()->BuildCommonGlobelDescriptor(descriptor_name, skin_animation_descriptor_desc, buffer_data, true);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancySkinAnimationControl::BuildPrivateBufferDescriptorUAV(
	std::vector<VirtualResourcePointer>& buffer_data,
	const LunarResourceSize& buffer_size,
	const LunarResourceSize& per_element_size,
	BindDescriptorPointer& descriptor_id
)
{
	LunarObjectID Frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	std::vector<BasicDescriptorDesc> skin_animation_descriptor_desc;
	BasicDescriptorDesc skin_animation_UAV_desc;
	skin_animation_UAV_desc.basic_descriptor_type = PancyDescriptorType::DescriptorTypeUnorderedAccessView;
	skin_animation_UAV_desc.unordered_access_view_desc = {};
	skin_animation_UAV_desc.unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.StructureByteStride = static_cast<UINT>(per_element_size);
	LunarResourceSize element_num = buffer_size / per_element_size;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(element_num);
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.FirstElement = 0;
	skin_animation_UAV_desc.unordered_access_view_desc.Buffer.CounterOffsetInBytes = 0;
	for (LunarObjectID i = 0; i < Frame_num; ++i)
	{
		skin_animation_descriptor_desc.push_back(skin_animation_UAV_desc);
	}
	auto check_error = PancyDescriptorHeapControl::GetInstance()->BuildCommonDescriptor(skin_animation_descriptor_desc, buffer_data, true, descriptor_id);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancySkinAnimationControl::BuildPrivateBufferDescriptorSRV(
	std::vector<VirtualResourcePointer>& buffer_data,
	const LunarResourceSize& buffer_size,
	const LunarResourceSize& per_element_size,
	BindDescriptorPointer& descriptor_id
)
{
	LunarObjectID Frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	std::vector<BasicDescriptorDesc> skin_animation_descriptor_desc;
	BasicDescriptorDesc skin_animation_SRV_desc;
	skin_animation_SRV_desc.basic_descriptor_type = PancyDescriptorType::DescriptorTypeShaderResourceView;
	LunarObjectID element_num = static_cast<LunarObjectID>(buffer_size / per_element_size);
	skin_animation_SRV_desc.shader_resource_view_desc = {};
	skin_animation_SRV_desc.shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	skin_animation_SRV_desc.shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.StructureByteStride = static_cast<UINT>(per_element_size);
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(element_num);;
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	skin_animation_SRV_desc.shader_resource_view_desc.Buffer.FirstElement = 0;
	for (LunarObjectID i = 0; i < Frame_num; ++i)
	{
		skin_animation_descriptor_desc.push_back(skin_animation_SRV_desc);
	}
	auto check_error = PancyDescriptorHeapControl::GetInstance()->BuildCommonDescriptor(skin_animation_descriptor_desc, buffer_data, true, descriptor_id);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}

LResult PancySkinAnimationControl::Create()
{
	LResult check_error;
	check_error = skin_mesh_animation_buffer_descriptor.Create(MaxSkinAnimationComputeNum);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = skin_mesh_offset_matrix_descriptor.Create(MaxSkinAnimationComputeNum);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = skin_mesh_parent_id_descriptor.Create(MaxSkinAnimationComputeNum);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//加载PSO
	check_error = PancyEffectGraphic::GetInstance()->GetPSO("assets\\pipeline\\pipline_state_object\\pso_skinmesh.json", PSO_skinmesh);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = PancyEffectGraphic::GetInstance()->GetPSO("assets\\pipeline\\pipline_state_object\\pso_skin_bonetree.json", PSO_skinmesh_skintree_desample);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = PancyEffectGraphic::GetInstance()->GetPSO("assets\\pipeline\\pipline_state_object\\pso_skin_matrix_combine.json", PSO_skinmesh_cluster_combine);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = PancyEffectGraphic::GetInstance()->GetPSO("assets\\pipeline\\pipline_state_object\\pso_skin_sample_aniamation.json", PSO_skinmesh_animation_interpolation);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//创建骨骼动画缓冲区
	std::vector<VirtualResourcePointer> skin_animation_buffer_data;
	std::vector<VirtualResourcePointer> bone_buffer_data1;
	std::vector<VirtualResourcePointer> bone_buffer_data2;
	std::vector<VirtualResourcePointer> bone_id_data;
	LunarObjectID Frame_num = PancyDx12DeviceBasic::GetInstance()->GetFrameNum();
	for (LunarObjectID i = 0; i < Frame_num; ++i)
	{
		skin_naimation_buffer[i] = new PancySkinAnimationBuffer(animation_buffer_size, bone_buffer_size);
		check_error = skin_naimation_buffer[i]->Create();
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		VirtualResourcePointer& skin_vertex_resource = skin_naimation_buffer[i]->GetSkinVertexResource();
		VirtualResourcePointer& bone_matrix_resource1 = skin_naimation_buffer[i]->GetBoneMatrixResource(0);
		VirtualResourcePointer& bone_matrix_resource2 = skin_naimation_buffer[i]->GetBoneMatrixResource(1);
		VirtualResourcePointer& bone_id_resource = skin_naimation_buffer[i]->GetBoneIDResource();
		skin_animation_buffer_data.push_back(skin_vertex_resource);
		bone_buffer_data1.push_back(bone_matrix_resource1);
		bone_buffer_data2.push_back(bone_matrix_resource2);
		bone_id_data.push_back(bone_id_resource);
	}
	//创建骨骼动画缓冲区的描述符
	check_error = BuildGlobelBufferDescriptorSRV("input_point", skin_animation_buffer_data, animation_buffer_size, sizeof(mesh_animation_data));
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = BuildGlobelBufferDescriptorUAV("mesh_anim_data", skin_animation_buffer_data, animation_buffer_size, sizeof(mesh_animation_data));
	if (!check_error.m_IsOK)
	{
		return check_error;
	}

	check_error = BuildPrivateBufferDescriptorSRV(bone_buffer_data1, bone_buffer_size, sizeof(DirectX::XMFLOAT4X4), buffer_descriptor_srv_id1);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = BuildPrivateBufferDescriptorSRV(bone_buffer_data2, bone_buffer_size, sizeof(DirectX::XMFLOAT4X4), buffer_descriptor_srv_id2);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = BuildPrivateBufferDescriptorUAV(bone_buffer_data1, bone_buffer_size, sizeof(DirectX::XMFLOAT4X4), buffer_descriptor_uav_id1);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = BuildPrivateBufferDescriptorUAV(bone_buffer_data2, bone_buffer_size, sizeof(DirectX::XMFLOAT4X4), buffer_descriptor_uav_id2);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}


	check_error = BuildGlobelBufferDescriptorSRV("bone_matrix_buffer", bone_buffer_data1, bone_buffer_size, sizeof(DirectX::XMFLOAT4X4));
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = BuildGlobelBufferDescriptorUAV("bone_matrix_output", bone_buffer_data1, bone_buffer_size, sizeof(DirectX::XMFLOAT4X4));
	if (!check_error.m_IsOK)
	{
		return check_error;
	}

	check_error = BuildGlobelBufferDescriptorSRV("compute_globel_id", bone_id_data, bone_buffer_size, sizeof(DirectX::XMUINT4));
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = BuildGlobelBufferDescriptorUAV("compute_globel_id_input", bone_id_data, bone_buffer_size, sizeof(DirectX::XMUINT4));
	if (!check_error.m_IsOK)
	{
		return check_error;
	}


	std::unordered_map<std::string, BindDescriptorPointer> bind_shader_resource_animation_sample;
	std::unordered_map<std::string, BindlessDescriptorPointer> bindless_shader_animation_sample;
	bindless_shader_animation_sample.insert(std::pair<std::string, BindlessDescriptorPointer>("animation_data", skin_mesh_animation_buffer_descriptor.GetDescriptorID()));
	check_error = RenderParamSystem::GetInstance()->GetCommonRenderParam(
		"assets\\pipeline\\pipline_state_object\\pso_skin_sample_aniamation.json",
		bind_shader_resource_animation_sample,
		bindless_shader_animation_sample,
		render_param_id_animation_sample
	);

	std::unordered_map<std::string, BindDescriptorPointer> bind_shader_resource_bone;
	std::unordered_map<std::string, BindlessDescriptorPointer> bindless_shader_resource_bone;
	for (int32_t bone_compute_index = 0; bone_compute_index < 10; ++bone_compute_index)
	{
		bind_shader_resource_bone.clear();
		bindless_shader_resource_bone.clear();
		if (bone_compute_index % 2 == 0)
		{
			bind_shader_resource_bone.insert(std::pair<std::string, BindDescriptorPointer>("bone_matrix_input", buffer_descriptor_srv_id1));
			bind_shader_resource_bone.insert(std::pair<std::string, BindDescriptorPointer>("bone_matrix_output", buffer_descriptor_uav_id2));
		}
		else
		{
			bind_shader_resource_bone.insert(std::pair<std::string, BindDescriptorPointer>("bone_matrix_input", buffer_descriptor_srv_id2));
			bind_shader_resource_bone.insert(std::pair<std::string, BindDescriptorPointer>("bone_matrix_output", buffer_descriptor_uav_id1));
		}
		bindless_shader_resource_bone.insert(std::pair<std::string, BindlessDescriptorPointer>("parent_globel_id", skin_mesh_parent_id_descriptor.GetDescriptorID()));
		check_error = RenderParamSystem::GetInstance()->GetCommonRenderParam(
			"assets\\pipeline\\pipline_state_object\\pso_skin_bonetree.json",
			bind_shader_resource_bone,
			bindless_shader_resource_bone,
			render_param_id_bone_compute[bone_compute_index]
		);
	}


	std::unordered_map<std::string, BindDescriptorPointer> bind_shader_resource_combine_mat;
	std::unordered_map<std::string, BindlessDescriptorPointer> bindless_shader_resource_combine_mat;
	bind_shader_resource_combine_mat.insert(std::pair<std::string, BindDescriptorPointer>("bone_matrix_input", buffer_descriptor_srv_id1));
	bindless_shader_resource_combine_mat.insert(std::pair<std::string, BindlessDescriptorPointer>("offset_matrix", skin_mesh_offset_matrix_descriptor.GetDescriptorID()));
	check_error = RenderParamSystem::GetInstance()->GetCommonRenderParam(
		"assets\\pipeline\\pipline_state_object\\pso_skin_matrix_combine.json",
		bind_shader_resource_combine_mat,
		bindless_shader_resource_combine_mat,
		render_param_id_combine_matrix
	);

	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
void PancySkinAnimationControl::ClearUsedBuffer()
{
	LunarObjectID now_frame_use = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	skin_naimation_buffer[now_frame_use]->ClearUsedBuffer();
	skin_animation_matrix_buffer.clear();
	skin_offset_matrix_buffer.clear();
	skin_parent_id_buffer.clear();
	skin_animation_matrix_descriptor.clear();
	skin_offset_matrix_descriptor.clear();
	skin_parent_id_descriptor.clear();
	skin_animation_message.clear();
	globel_bone_matrix_size = 0;
	globel_offset_matrix_size = 0;
	globel_parent_id_size = 0;
}
LResult PancySkinAnimationControl::BuildAnimationBlock(
	const LunarResourceSize& vertex_num,
	LunarObjectID& block_id,
	SkinAnimationBlock& new_animation_block
)
{
	LResult check_error;
	LunarObjectID now_frame_use = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	check_error = skin_naimation_buffer[now_frame_use]->BuildAnimationBlock(vertex_num, block_id, new_animation_block);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
LResult PancySkinAnimationControl::BuildBoneBlock(
	VirtualResourcePointer& animation_matrix_buffer,
	VirtualResourcePointer& offset_matrix_buffer,
	VirtualResourcePointer& parent_id_buffer,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& animation_buffer_descriptor,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& offset_matrix_descriptor,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& parent_id_descriptor,
	const LunarResourceSize& animation_resample_num,
	const LunarResourceSize& animation_start_pos,
	const LunarResourceSize& offset_matrix_num,
	const LunarResourceSize& bone_parent_id_num,
	const LunarResourceSize& bone_matrix_num,
	LunarObjectID& block_id,
	SkinAnimationBlock& new_bone_block
)
{
	LResult check_error;

	//根据当前的帧号申请一片缓冲区
	LunarObjectID now_frame_use = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	check_error = skin_naimation_buffer[now_frame_use]->BuildBoneBlock(bone_matrix_num, block_id, new_bone_block);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}


	//存储动画数据缓冲区与偏移矩阵缓冲区
	skin_animation_matrix_buffer.push_back(animation_matrix_buffer);
	skin_offset_matrix_buffer.push_back(offset_matrix_buffer);
	skin_parent_id_buffer.push_back(parent_id_buffer);

	skin_animation_matrix_descriptor.push_back(animation_buffer_descriptor);
	skin_offset_matrix_descriptor.push_back(offset_matrix_descriptor);
	skin_parent_id_descriptor.push_back(parent_id_descriptor);

	AnimationStartMessage new_animation_message;
	new_animation_message.drawcall_id = static_cast<LunarObjectID>(skin_animation_message.size());
	new_animation_message.animation_resample_num = static_cast<LunarObjectID>(animation_resample_num);
	new_animation_message.animation_start_pos = static_cast<LunarObjectID>(animation_start_pos);

	new_animation_message.offset_matrix_offset_from_begin = globel_offset_matrix_size;
	new_animation_message.offset_matrix_num = static_cast<LunarObjectID>(offset_matrix_num);

	new_animation_message.bone_matrix_offset_from_begin = globel_bone_matrix_size;
	new_animation_message.bone_matrix_num = static_cast<LunarObjectID>(bone_matrix_num);
	new_animation_message.bone_buffer_block_id = block_id;

	new_animation_message.bone_parent_id_offset_from_begin = globel_parent_id_size;
	new_animation_message.bone_parent_id_num = static_cast<LunarObjectID>(bone_parent_id_num);

	globel_offset_matrix_size += new_animation_message.offset_matrix_num;
	globel_bone_matrix_size += new_animation_message.bone_matrix_num;
	globel_parent_id_size += static_cast<LunarObjectID>(bone_parent_id_num);

	skin_animation_message.push_back(new_animation_message);

	return g_Succeed;
}
LResult PancySkinAnimationControl::ComputeBoneMatrix(const float& play_time)
{
	LResult check_error;
	//重新绑定三组bindless描述符
	skin_mesh_animation_buffer_descriptor.BuildShaderResourceViewFromHead(skin_animation_matrix_descriptor, skin_animation_matrix_buffer);
	skin_mesh_parent_id_descriptor.BuildShaderResourceViewFromHead(skin_parent_id_descriptor, skin_parent_id_buffer);
	skin_mesh_offset_matrix_descriptor.BuildShaderResourceViewFromHead(skin_offset_matrix_descriptor, skin_offset_matrix_buffer);
	//创建第一个shader
	PancyRenderCommandList* m_commandList_animation_sample;
	PancyThreadIdGPU commdlist_id_animation_sample;
	ID3D12PipelineState* pso_data;
	check_error = RenderParamSystem::GetInstance()->GetPsoData(render_param_id_animation_sample, &pso_data);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = ThreadPoolGPUControl::GetInstance()->GetMainContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)->GetEmptyRenderlist(pso_data, &m_commandList_animation_sample, commdlist_id_animation_sample);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//开启UAV状态
	int32_t now_render_num = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	auto bone_matrix_res = GetBufferResourceData(skin_naimation_buffer[now_render_num]->GetBoneMatrixResource(0), check_error);
	check_error = bone_matrix_res->ResourceBarrier(m_commandList_animation_sample, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	auto compute_globel_id_res = GetBufferResourceData(skin_naimation_buffer[now_render_num]->GetBoneIDResource(), check_error);
	check_error = compute_globel_id_res->ResourceBarrier(m_commandList_animation_sample, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	for (int32_t drawcall = 0; drawcall < skin_animation_message.size(); ++drawcall)
	{
		DirectX::XMUINT4 animation_id_param;
		animation_id_param.x = drawcall;
		animation_id_param.y = skin_animation_message[drawcall].bone_matrix_num;
		animation_id_param.z = skin_animation_message[drawcall].animation_resample_num;
		DirectX::XMUINT4 animation_begin_param;
		animation_begin_param.x = skin_animation_message[drawcall].bone_matrix_offset_from_begin;
		animation_begin_param.y = skin_animation_message[drawcall].animation_start_pos;
		DirectX::XMFLOAT4 animation_play_param;
		animation_play_param.x = play_time;
		RenderParamSystem::GetInstance()->SetCbufferUint4(render_param_id_animation_sample, "animation_sample_param", "animation_id_param", animation_id_param, 0);
		RenderParamSystem::GetInstance()->SetCbufferUint4(render_param_id_animation_sample, "animation_sample_param", "animation_begin_param", animation_begin_param, 0);
		RenderParamSystem::GetInstance()->SetCbufferFloat4(render_param_id_animation_sample, "animation_sample_param", "animation_play_param", animation_play_param, 0);
		check_error = RenderParamSystem::GetInstance()->AddRenderParamToCommandList(render_param_id_animation_sample, m_commandList_animation_sample, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//制作计算命令
		LunarObjectID thread_group_size;
		thread_group_size = skin_animation_message[drawcall].bone_matrix_num / 64;
		if (skin_animation_message[drawcall].bone_matrix_num % 64 != 0)
		{
			thread_group_size += 1;
		}
		m_commandList_animation_sample->GetCommandList()->Dispatch(thread_group_size, 1, 1);

	}
	//关闭UAV状态
	check_error = bone_matrix_res->ResourceBarrier(m_commandList_animation_sample, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = compute_globel_id_res->ResourceBarrier(m_commandList_animation_sample, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	m_commandList_animation_sample->UnlockPrepare();
	check_error = ThreadPoolGPUControl::GetInstance()->GetMainContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)->SubmitRenderlist(1, &commdlist_id_animation_sample);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//创建第二个shader
	PancyRenderCommandList* m_commandList_bone_tree;
	PancyThreadIdGPU commdlist_id_bone_tree;
	ID3D12PipelineState* pso_data_bone_tree;
	check_error = RenderParamSystem::GetInstance()->GetPsoData(render_param_id_bone_compute[0], &pso_data_bone_tree);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = ThreadPoolGPUControl::GetInstance()->GetMainContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)->GetEmptyRenderlist(pso_data_bone_tree, &m_commandList_bone_tree, commdlist_id_bone_tree);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = bone_matrix_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = compute_globel_id_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}

	auto bone_matrix2_res = GetBufferResourceData(skin_naimation_buffer[now_render_num]->GetBoneMatrixResource(1), check_error);
	check_error = bone_matrix2_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	for (int32_t drawcall = 0; drawcall < 6; ++drawcall)
	{
		DirectX::XMUINT4 draw_pass_id_param;
		draw_pass_id_param.x = drawcall;
		draw_pass_id_param.y = globel_bone_matrix_size;
		DirectX::XMUINT4 bone_id_param;
		bone_id_param.x = 8;

		RenderParamSystem::GetInstance()->SetCbufferUint4(render_param_id_bone_compute[drawcall], "animation_sample_param", "draw_pass_id_param", draw_pass_id_param, 0);
		RenderParamSystem::GetInstance()->SetCbufferUint4(render_param_id_bone_compute[drawcall], "animation_sample_param", "bone_id_param", bone_id_param, 0);
		check_error = RenderParamSystem::GetInstance()->AddRenderParamToCommandList(render_param_id_bone_compute[drawcall], m_commandList_bone_tree, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//制作计算命令
		LunarObjectID thread_group_size;
		thread_group_size = globel_bone_matrix_size / 64;
		if (globel_bone_matrix_size % 64 != 0)
		{
			thread_group_size += 1;
		}
		m_commandList_bone_tree->GetCommandList()->Dispatch(thread_group_size, 1, 1);
		if (drawcall % 2 == 0)
		{
			check_error = bone_matrix_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			check_error = bone_matrix2_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		else
		{
			check_error = bone_matrix_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			check_error = bone_matrix2_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
	}


	check_error = bone_matrix2_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = compute_globel_id_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = bone_matrix_res->ResourceBarrier(m_commandList_bone_tree, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}

	m_commandList_bone_tree->UnlockPrepare();
	check_error = ThreadPoolGPUControl::GetInstance()->GetMainContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)->SubmitRenderlist(1, &commdlist_id_bone_tree);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//创建第三个shader
	PancyRenderCommandList* m_commandList_combine;
	PancyThreadIdGPU commdlist_id_combine;
	ID3D12PipelineState* pso_data_combine;
	check_error = RenderParamSystem::GetInstance()->GetPsoData(render_param_id_combine_matrix, &pso_data_combine);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = ThreadPoolGPUControl::GetInstance()->GetMainContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)->GetEmptyRenderlist(pso_data_combine, &m_commandList_combine, commdlist_id_combine);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = bone_matrix_res->ResourceBarrier(m_commandList_combine, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = bone_matrix2_res->ResourceBarrier(m_commandList_combine, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	DirectX::XMUINT4 bone_size_param;
	bone_size_param.x = globel_bone_matrix_size;
	RenderParamSystem::GetInstance()->SetCbufferUint4(render_param_id_combine_matrix, "animation_sample_param", "bone_size_param", bone_size_param, 0);
	check_error = RenderParamSystem::GetInstance()->AddRenderParamToCommandList(render_param_id_combine_matrix, m_commandList_combine, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//制作计算命令
	LunarObjectID thread_group_size;
	thread_group_size = globel_bone_matrix_size / 64;
	if (globel_bone_matrix_size % 64 != 0)
	{
		thread_group_size += 1;
	}
	m_commandList_combine->GetCommandList()->Dispatch(thread_group_size, 1, 1);
	check_error = bone_matrix_res->ResourceBarrier(m_commandList_combine, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = bone_matrix2_res->ResourceBarrier(m_commandList_combine, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	m_commandList_combine->UnlockPrepare();
	check_error = ThreadPoolGPUControl::GetInstance()->GetMainContex()->GetThreadPool(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT)->SubmitRenderlist(1, &commdlist_id_combine);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
//填充渲染commandlist
LResult PancySkinAnimationControl::BuildCommandList(
	const LunarObjectID& bone_block_ID,
	VirtualResourcePointer& mesh_buffer,
	const LunarObjectID& vertex_num,
	const PancyRenderParamID& render_param_id,
	const LunarResourceSize& matrix_num,
	SkinAnimationBlock& animation_block_pos,
	PancyRenderCommandList* m_commandList_skin
)
{
	LResult check_error;
	LunarObjectID skin_animation_block_ID;
	AnimationStartMessage bone_block_pos;
	//根据渲染模型的顶点数据请求一块动画存储显存
	check_error = BuildAnimationBlock(vertex_num, skin_animation_block_ID, animation_block_pos);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//获取当前drawcall之前申请的骨骼数据
	if (skin_animation_message[bone_block_ID].bone_matrix_num != matrix_num + 2)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "build skin mesh commondlist error: matrix num dismatch", error_message);

		return error_message;
	}
	bone_block_pos = skin_animation_message[bone_block_ID];
	//获取渲染描述符
	check_error = RenderParamSystem::GetInstance()->AddRenderParamToCommandList(render_param_id, m_commandList_skin, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//填充常量缓冲区
	DirectX::XMUINT4 data_offset;
	DirectX::XMUINT4 data_num;
	data_offset.x = static_cast<uint32_t>(bone_block_pos.bone_matrix_offset_from_begin);
	data_offset.y = static_cast<uint32_t>(animation_block_pos.start_pos);
	data_num.x = vertex_num;
	data_num.y = static_cast<uint32_t>(matrix_num);
	check_error = RenderParamSystem::GetInstance()->SetCbufferUint4(render_param_id, "per_object", "data_offset", data_offset, 0);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = RenderParamSystem::GetInstance()->SetCbufferUint4(render_param_id, "per_object", "data_num", data_num, 0);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//修改当前输入顶点资源的使用格式
	auto now_resource_data = mesh_buffer.GetResourceData();
	auto vertex_input_gpu_buffer = GetBufferResourceData(mesh_buffer, check_error);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = vertex_input_gpu_buffer->ResourceBarrier(m_commandList_skin, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//修改当前输出顶点资源的使用格式
	LunarObjectID now_frame = PancyDx12DeviceBasic::GetInstance()->GetNowFrame();
	VirtualResourcePointer& bone_matrix_resource = skin_naimation_buffer[now_frame]->GetBoneMatrixResource(1);
	VirtualResourcePointer& skin_vertex_resource = skin_naimation_buffer[now_frame]->GetSkinVertexResource();
	auto bone_input_buffer = GetBufferResourceData(bone_matrix_resource, check_error);
	check_error = bone_input_buffer->ResourceBarrier(m_commandList_skin, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	auto vertex_output_gpu_buffer = GetBufferResourceData(skin_vertex_resource, check_error);
	check_error = vertex_output_gpu_buffer->ResourceBarrier(m_commandList_skin, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	//提交计算命令
	LunarObjectID thread_group_size;
	thread_group_size = vertex_num / threadBlockSize;
	if (vertex_num % threadBlockSize != 0)
	{
		thread_group_size += 1;
	}
	m_commandList_skin->GetCommandList()->Dispatch(thread_group_size, 1, 1);
	//还原资源状态
	check_error = bone_input_buffer->ResourceBarrier(m_commandList_skin, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = vertex_input_gpu_buffer->ResourceBarrier(m_commandList_skin, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = vertex_output_gpu_buffer->ResourceBarrier(m_commandList_skin, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	m_commandList_skin->UnlockPrepare();
	return g_Succeed;
}