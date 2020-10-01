#include"PancyModelBasic.h"
#include "core/misc/path.h"

using namespace LunarEngine;
//模型部件
PancyRenderMesh::PancyRenderMesh()
{
	model_mesh = NULL;
}
PancyRenderMesh::~PancyRenderMesh()
{
	if (model_mesh != NULL)
	{
		delete model_mesh;
	}
}
//模型类
PancyBasicModel::PancyBasicModel()
{
}
LResult PancyBasicModel::Create(const std::string& resource_name)
{
	Json::Value root_value;
	auto check_error = PancyJsonTool::GetInstance()->LoadJsonFile(resource_name, root_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	check_error = InitResource(root_value, resource_name);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
PancyBasicModel::~PancyBasicModel()
{
	for (auto data_submodel = model_resource_list.begin(); data_submodel != model_resource_list.end(); ++data_submodel)
	{
		delete* data_submodel;
	}
	model_resource_list.clear();
}
//骨骼动画处理函数
LResult PancyBasicModel::LoadSkinTree(const std::string& filename)
{
	instream.open(filename, ios::binary);
	if (!instream.is_open())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "open file " + filename + " error", error_message);

		return error_message;
	}
	//读取偏移矩阵
	instream.read(reinterpret_cast<char*>(&bone_num), sizeof(bone_num));
	instream.read(reinterpret_cast<char*>(offset_matrix_array), bone_num * sizeof(DirectX::XMFLOAT4X4));
	//先读取第一个入栈符
	char data[11];
	instream.read(reinterpret_cast<char*>(data), sizeof(data));
	//递归重建骨骼树
	bone_object_num = bone_num;
	auto check_error = ReadBoneTree(root_id);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	bone_parent_data[root_id] = 0;
	//关闭文件
	instream.close();
	return g_Succeed;
}
LResult PancyBasicModel::BuildBoneDataPerFrame(LunarObjectID& bone_block_id, SkinAnimationBlock& new_animation_block)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC animation_buffer_desc = {};
	animation_buffer_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	animation_buffer_desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
	animation_buffer_desc.Buffer.StructureByteStride = sizeof(DirectX::XMFLOAT4);
	animation_buffer_desc.Buffer.NumElements = static_cast<UINT>(animation_buffer_size);
	animation_buffer_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	animation_buffer_desc.Buffer.FirstElement = 0;

	D3D12_SHADER_RESOURCE_VIEW_DESC boneoffset_buffer_desc = {};
	boneoffset_buffer_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	boneoffset_buffer_desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
	boneoffset_buffer_desc.Buffer.StructureByteStride = sizeof(DirectX::XMFLOAT4X4);
	boneoffset_buffer_desc.Buffer.NumElements = static_cast<UINT>(boneoffset_buffer_size);
	boneoffset_buffer_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	boneoffset_buffer_desc.Buffer.FirstElement = 0;

	D3D12_SHADER_RESOURCE_VIEW_DESC bonetree_buffer_desc = {};
	bonetree_buffer_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	bonetree_buffer_desc.ViewDimension = D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
	bonetree_buffer_desc.Buffer.StructureByteStride = sizeof(uint32_t);
	bonetree_buffer_desc.Buffer.NumElements = static_cast<UINT>(bonetree_buffer_size);
	bonetree_buffer_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	bonetree_buffer_desc.Buffer.FirstElement = 0;

	PancySkinAnimationControl::GetInstance()->BuildBoneBlock(
		model_animation_buffer,
		model_boneoffset_buffer,
		model_bonetree_buffer,
		animation_buffer_desc,
		boneoffset_buffer_desc,
		bonetree_buffer_desc,
		animation_resample_size[0],
		animation_start_offset[0],
		boneoffset_buffer_size,
		bonetree_buffer_size,
		boneoffset_buffer_size,
		bone_block_id,
		new_animation_block
	);
	return g_Succeed;
}
//todo::取消使用这个结构
struct skin_tree
{
	char bone_ID[128];
	int bone_number;
	DirectX::XMFLOAT4X4 basic_matrix;
	DirectX::XMFLOAT4X4 animation_matrix;
	DirectX::XMFLOAT4X4 now_matrix;
	skin_tree* brother;
	skin_tree* son;
	skin_tree()
	{
		bone_ID[0] = '\0';
		bone_number = NouseBoneStruct;
		brother = NULL;
		son = NULL;
		DirectX::XMStoreFloat4x4(&basic_matrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&animation_matrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&now_matrix, DirectX::XMMatrixIdentity());
	}
};
LResult PancyBasicModel::ReadBoneTree(int32_t& now_build_id)
{
	char data[11];
	skin_tree now_bone_data;
	instream.read(reinterpret_cast<char*>(&now_bone_data), sizeof(now_bone_data));
	//根据读取到的骨骼节点创建一个骨骼结构并赋予编号
	bone_struct new_bone;
	bool if_used_skin = false;;
	if (now_bone_data.bone_number == NouseBoneStruct)
	{
		//当前骨骼并未用于蒙皮信息,重新生成一个ID号
		now_build_id = bone_object_num;
		bone_object_num += 1;
	}
	else
	{
		if_used_skin = true;
		now_build_id = now_bone_data.bone_number;
	}
	new_bone.bone_name = now_bone_data.bone_ID;
	new_bone.bone_ID_son = NouseBoneStruct;
	new_bone.bone_ID_brother = NouseBoneStruct;
	new_bone.if_used_for_skin = if_used_skin;
	bone_tree_data.insert(std::pair<int32_t, bone_struct>(now_build_id, new_bone));
	bone_name_index.insert(std::pair<std::string, int32_t>(new_bone.bone_name, now_build_id));
	instream.read(data, sizeof(data));
	while (strcmp(data, "*heaphead*") == 0)
	{
		//入栈符号，代表子节点
		int32_t now_son_ID = NouseBoneStruct;
		//递归创建子节点
		auto check_error = ReadBoneTree(now_son_ID);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		auto now_parent_data = bone_tree_data.find(now_build_id);
		auto now_data = bone_tree_data.find(now_son_ID);
		if (now_parent_data == bone_tree_data.end())
		{
			//传入的父节点不合理
			LResult error_message;
			LunarDebugLogError(E_FAIL, "could not find Parent bone ID " + std::to_string(now_build_id), error_message);

			return error_message;
		}
		else if (now_data == bone_tree_data.end())
		{
			//生成的子节点不合理
			LResult error_message;
			LunarDebugLogError(E_FAIL, "could not find Son Bone ID " + std::to_string(now_son_ID), error_message);

			return error_message;
		}
		else
		{
			//当前节点的兄弟节点存储之前父节点的子节点
			now_data->second.bone_ID_brother = now_parent_data->second.bone_ID_son;
			//之前父节点的子节点变为当前节点
			now_parent_data->second.bone_ID_son = now_son_ID;
			//记录当前节点的父节点
			bone_parent_data[now_son_ID] = now_build_id + 1;
			instream.read(data, sizeof(data));
		}
	}
	return g_Succeed;
}
void PancyBasicModel::Interpolate(quaternion_animation& pOut, const quaternion_animation& pStart, const quaternion_animation& pEnd, const float& pFactor)
{
	float cosom = pStart.main_key[0] * pEnd.main_key[0] + pStart.main_key[1] * pEnd.main_key[1] + pStart.main_key[2] * pEnd.main_key[2] + pStart.main_key[3] * pEnd.main_key[3];
	quaternion_animation end = pEnd;
	if (cosom < static_cast<float>(0.0))
	{
		cosom = -cosom;
		end.main_key[0] = -end.main_key[0];
		end.main_key[1] = -end.main_key[1];
		end.main_key[2] = -end.main_key[2];
		end.main_key[3] = -end.main_key[3];
	}
	float sclp, sclq;
	if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001))
	{
		float omega, sinom;
		omega = acos(cosom);
		sinom = sin(omega);
		sclp = sin((static_cast<float>(1.0) - pFactor) * omega) / sinom;
		sclq = sin(pFactor * omega) / sinom;
	}
	else
	{
		sclp = static_cast<float>(1.0) - pFactor;
		sclq = pFactor;
	}

	pOut.main_key[0] = sclp * pStart.main_key[0] + sclq * end.main_key[0];
	pOut.main_key[1] = sclp * pStart.main_key[1] + sclq * end.main_key[1];
	pOut.main_key[2] = sclp * pStart.main_key[2] + sclq * end.main_key[2];
	pOut.main_key[3] = sclp * pStart.main_key[3] + sclq * end.main_key[3];
}
void PancyBasicModel::Interpolate(vector_animation& pOut, const vector_animation& pStart, const vector_animation& pEnd, const float& pFactor)
{
	for (int i = 0; i < 3; ++i)
	{
		pOut.main_key[i] = pStart.main_key[i] + pFactor * (pEnd.main_key[i] - pStart.main_key[i]);
	}
}
void PancyBasicModel::FindAnimStEd(const float& input_time, int& st, int& ed, const std::vector<quaternion_animation>& input)
{
	if (input_time < 0)
	{
		st = 0;
		ed = 0;
		return;
	}
	if (input_time > input[input.size() - 1].time)
	{
		st = static_cast<int>(input.size()) - 1;
		ed = static_cast<int>(input.size()) - 1;
		return;
	}
	for (int i = 0; i < input.size() - 1; ++i)
	{
		if (input_time >= input[i].time && input_time <= input[i + 1].time)
		{
			st = i;
			ed = i + 1;
			return;
		}
	}
	st = static_cast<int>(input.size()) - 1;
	ed = static_cast<int>(input.size()) - 1;
}
void PancyBasicModel::FindAnimStEd(const float& input_time, int& st, int& ed, const std::vector<vector_animation>& input)
{
	if (input_time < 0)
	{
		st = 0;
		ed = 0;
		return;
	}
	if (input_time > input[input.size() - 1].time)
	{
		st = static_cast<int>(input.size()) - 1;
		ed = static_cast<int>(input.size()) - 1;
		return;
	}
	for (int i = 0; i < input.size() - 1; ++i)
	{
		if (input_time >= input[i].time && input_time <= input[i + 1].time)
		{
			st = i;
			ed = i + 1;
			return;
		}
	}
	st = static_cast<int>(input.size()) - 1;
	ed = static_cast<int>(input.size()) - 1;
}
LResult PancyBasicModel::GetBoneByAnimation(
	const LunarResourceID& animation_ID,
	const float& animation_time,
	std::vector<DirectX::XMFLOAT4X4>& bone_final_matrix
)
{
	LResult check_error;
	//先判断模型数据是否支持骨骼动画
	if (!if_skinmesh)
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "model: don't have skin mesh message", error_message);

		return error_message;
	}
	std::vector<DirectX::XMFLOAT4X4> matrix_animation_save;
	matrix_animation_save.resize(bone_object_num);
	for (int i = 0; i < bone_object_num; ++i)
	{
		//使用单位矩阵将动画矩阵清空
		DirectX::XMStoreFloat4x4(&matrix_animation_save[i], DirectX::XMMatrixIdentity());
	}
	check_error = UpdateAnimData(animation_ID, animation_time, matrix_animation_save);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	DirectX::XMFLOAT4X4 matrix_identi;
	DirectX::XMStoreFloat4x4(&matrix_identi, DirectX::XMMatrixIdentity());

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//开辟一个临时存储所有骨骼递归结果的矩阵数组。由于最终输出的数据仅包
	//含蒙皮骨骼，因而需要额外的空间存储非蒙皮骨骼的中间信息。
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::vector<DirectX::XMFLOAT4X4> matrix_combine_save;
	matrix_combine_save.resize(bone_object_num);
	for (int i = 0; i < bone_object_num; ++i)
	{
		DirectX::XMStoreFloat4x4(&matrix_combine_save[i], DirectX::XMMatrixIdentity());
	}
	std::vector<DirectX::XMFLOAT4X4> matrix_out_save;
	matrix_out_save.resize(bone_num);
	bone_final_matrix.resize(bone_num);
	auto check_matrix0 = XMLoadFloat4x4(&matrix_animation_save[0]);
	auto check_matrix3 = XMLoadFloat4x4(&matrix_animation_save[3]);
	auto check_matrix4 = XMLoadFloat4x4(&matrix_animation_save[4]);
	auto check_matrix20 = XMLoadFloat4x4(&matrix_animation_save[20]);

	auto check_matrix22 = XMLoadFloat4x4(&matrix_animation_save[22]);
	auto check_matrix11 = XMLoadFloat4x4(&matrix_animation_save[11]);
	auto check_matrix33 = XMLoadFloat4x4(&matrix_animation_save[33]);
	auto check_matrix32 = XMLoadFloat4x4(&matrix_animation_save[32]);
	DirectX::XMFLOAT4X4 mat_line1, mat_line2, mat_line3;
	XMStoreFloat4x4(&mat_line1, check_matrix0 * check_matrix3);
	XMStoreFloat4x4(&mat_line2, check_matrix4 * check_matrix20);
	XMStoreFloat4x4(&mat_line3, check_matrix0 * check_matrix3 * check_matrix4 * check_matrix20);

	DirectX::XMFLOAT4X4 mat_line4, mat_line5, mat_line6;

	XMStoreFloat4x4(&mat_line4, check_matrix22 * check_matrix11);
	XMStoreFloat4x4(&mat_line5, check_matrix33 * check_matrix32);
	XMStoreFloat4x4(&mat_line6, check_matrix22 * check_matrix11 * check_matrix33 * check_matrix32);
	UpdateRoot(root_id, matrix_identi, matrix_animation_save, matrix_combine_save, matrix_out_save);
	//将更新后的动画矩阵做偏移
	for (int i = 0; i < bone_num; ++i)
	{
		//使用单位矩阵将混合矩阵清空
		DirectX::XMStoreFloat4x4(&bone_final_matrix[i], DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&offset_matrix_array[i]) * DirectX::XMLoadFloat4x4(&matrix_out_save[i])));
	}
	return g_Succeed;
}
LResult PancyBasicModel::UpdateAnimData(
	const LunarResourceID& animation_ID,
	const float& time_in,
	std::vector<DirectX::XMFLOAT4X4>& matrix_out
)
{
	//根据动画的ID号查找对应的动画数据
	auto skin_anim_data = skin_animation_map.find(animation_ID);
	if (skin_anim_data == skin_animation_map.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find skin_animation ID " + std::to_string(animation_ID), error_message);

		return error_message;
	}
	animation_set now_animation_use = skin_anim_data->second;
	float input_time = time_in * now_animation_use.animation_length;
	for (int i = 0; i < now_animation_use.data_animition.size(); ++i)
	{
		animation_data now = now_animation_use.data_animition[i];
		DirectX::XMMATRIX rec_trans, rec_scal;
		DirectX::XMFLOAT4X4 rec_rot;

		int start_anim, end_anim;
		FindAnimStEd(input_time, start_anim, end_anim, now.rotation_key);
		//四元数插值并寻找变换矩阵
		quaternion_animation rotation_now;
		if (start_anim == end_anim || end_anim >= now.rotation_key.size())
		{
			rotation_now = now.rotation_key[start_anim];
		}
		else
		{
			Interpolate(rotation_now, now.rotation_key[start_anim], now.rotation_key[end_anim], (input_time - now.rotation_key[start_anim].time) / (now.rotation_key[end_anim].time - now.rotation_key[start_anim].time));
		}
		GetQuatMatrix(rec_rot, rotation_now);
		//缩放变换
		FindAnimStEd(input_time, start_anim, end_anim, now.scaling_key);
		vector_animation scalling_now;
		if (start_anim == end_anim)
		{
			scalling_now = now.scaling_key[start_anim];
		}
		else
		{
			Interpolate(scalling_now, now.scaling_key[start_anim], now.scaling_key[end_anim], (input_time - now.scaling_key[start_anim].time) / (now.scaling_key[end_anim].time - now.scaling_key[start_anim].time));
		}
		rec_scal = DirectX::XMMatrixScaling(scalling_now.main_key[0], scalling_now.main_key[1], scalling_now.main_key[2]);
		//平移变换
		FindAnimStEd(input_time, start_anim, end_anim, now.translation_key);
		vector_animation translation_now;
		if (start_anim == end_anim)
		{
			translation_now = now.translation_key[start_anim];
		}
		else
		{
			Interpolate(translation_now, now.translation_key[start_anim], now.translation_key[end_anim], (input_time - now.translation_key[start_anim].time) / (now.translation_key[end_anim].time - now.translation_key[start_anim].time));
		}
		rec_trans = DirectX::XMMatrixTranslation(translation_now.main_key[0], translation_now.main_key[1], translation_now.main_key[2]);
		//检测当前动画节点对应的骨骼数据是否还正常
		auto bone_data = bone_tree_data.find(now.bone_ID);
		if (bone_data == bone_tree_data.end())
		{
			LResult error_message;
			LunarDebugLogError(E_FAIL, "could not find Bone ID " + std::to_string(now.bone_ID) + " in model: ", error_message);

			return error_message;
		}
		XMStoreFloat4x4(&matrix_out[now.bone_ID], rec_scal * XMLoadFloat4x4(&rec_rot) * rec_trans);
	}
	return g_Succeed;
}
LResult PancyBasicModel::UpdateRoot(
	int32_t root_id,
	const DirectX::XMFLOAT4X4& matrix_parent,
	const std::vector<DirectX::XMFLOAT4X4>& matrix_animation,
	std::vector<DirectX::XMFLOAT4X4>& matrix_combine_save,
	std::vector<DirectX::XMFLOAT4X4>& matrix_out
)
{
	LResult check_error;
	//查找当前的父节点是否存在
	auto now_root_bone_data = bone_tree_data.find(root_id);
	if (now_root_bone_data == bone_tree_data.end())
	{
		LResult error_message;
		LunarDebugLogError(E_FAIL, "could not find bone ID" + std::to_string(root_id) + " in model: ", error_message);

		return error_message;
	}
	//获取当前骨骼的动画矩阵
	DirectX::XMMATRIX rec = DirectX::XMLoadFloat4x4(&matrix_animation[root_id]);
	DirectX::XMStoreFloat4x4(&matrix_combine_save[root_id], rec * DirectX::XMLoadFloat4x4(&matrix_parent));
	//对于蒙皮需要的骨骼，将其导出
	if (now_root_bone_data->second.if_used_for_skin)
	{
		matrix_out[root_id] = matrix_combine_save[root_id];
	}
	//更新兄弟节点及子节点
	if (now_root_bone_data->second.bone_ID_brother != NouseBoneStruct)
	{
		check_error = UpdateRoot(now_root_bone_data->second.bone_ID_brother, matrix_parent, matrix_animation, matrix_combine_save, matrix_out);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	if (now_root_bone_data->second.bone_ID_son != NouseBoneStruct)
	{
		check_error = UpdateRoot(now_root_bone_data->second.bone_ID_son, matrix_combine_save[root_id], matrix_animation, matrix_combine_save, matrix_out);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
	}
	return g_Succeed;
}
void PancyBasicModel::GetQuatMatrix(DirectX::XMFLOAT4X4& resMatrix, const quaternion_animation& pOut)
{
	resMatrix._11 = static_cast<float>(1.0) - static_cast<float>(2.0) * (pOut.main_key[1] * pOut.main_key[1] + pOut.main_key[2] * pOut.main_key[2]);
	resMatrix._21 = static_cast<float>(2.0) * (pOut.main_key[0] * pOut.main_key[1] - pOut.main_key[2] * pOut.main_key[3]);
	resMatrix._31 = static_cast<float>(2.0) * (pOut.main_key[0] * pOut.main_key[2] + pOut.main_key[1] * pOut.main_key[3]);
	resMatrix._41 = 0.0f;

	resMatrix._12 = static_cast<float>(2.0) * (pOut.main_key[0] * pOut.main_key[1] + pOut.main_key[2] * pOut.main_key[3]);
	resMatrix._22 = static_cast<float>(1.0) - static_cast<float>(2.0) * (pOut.main_key[0] * pOut.main_key[0] + pOut.main_key[2] * pOut.main_key[2]);
	resMatrix._32 = static_cast<float>(2.0) * (pOut.main_key[1] * pOut.main_key[2] - pOut.main_key[0] * pOut.main_key[3]);
	resMatrix._42 = 0.0f;

	resMatrix._13 = static_cast<float>(2.0) * (pOut.main_key[0] * pOut.main_key[2] - pOut.main_key[1] * pOut.main_key[3]);
	resMatrix._23 = static_cast<float>(2.0) * (pOut.main_key[1] * pOut.main_key[2] + pOut.main_key[0] * pOut.main_key[3]);
	resMatrix._33 = static_cast<float>(1.0) - static_cast<float>(2.0) * (pOut.main_key[0] * pOut.main_key[0] + pOut.main_key[1] * pOut.main_key[1]);
	resMatrix._43 = 0.0f;

	resMatrix._14 = 0.0f;
	resMatrix._24 = 0.0f;
	resMatrix._34 = 0.0f;
	resMatrix._44 = 1.0f;
}
LunarObjectID PancyBasicModel::FindParentByLayer(
	const LunarObjectID& bone_id,
	const LunarObjectID& layer,
	const std::vector<uint32_t>& bone_node_parent_data
) {
	LunarObjectID now_parent_id = bone_id;
	for (LunarObjectID now_layer = 0; now_layer < layer; ++now_layer)
	{
		if (now_parent_id == bone_node_parent_data[now_parent_id])
		{
			return now_parent_id;
		}
		else
		{
			now_parent_id = bone_node_parent_data[now_parent_id];
		}
	}
	return now_parent_id;
}
//资源加载
LResult PancyBasicModel::InitResource(const Json::Value& root_value, const std::string& resource_name)
{
	LResult check_error;
	LString path_name = "";
	LString file_name = "";
	LString tile_name = "";
	LPathUtil::DivideFilePath(resource_name, path_name, file_name, tile_name);
	pancy_json_value rec_value;
	//是否包含骨骼动画
	check_error = PancyJsonTool::GetInstance()->GetJsonData(resource_name, root_value, "IfHaveSkinAnimation", pancy_json_data_type::json_data_bool, rec_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	if_skinmesh = rec_value.bool_value;
	//是否包含顶点动画
	check_error = PancyJsonTool::GetInstance()->GetJsonData(resource_name, root_value, "IfHavePoinAnimation", pancy_json_data_type::json_data_bool, rec_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	if_pointmesh = rec_value.bool_value;
	//读取模型的网格数据
	int32_t model_part_num;
	check_error = PancyJsonTool::GetInstance()->GetJsonData(resource_name, root_value, "model_num", pancy_json_data_type::json_data_int, rec_value);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	model_part_num = rec_value.int_value;
	for (int i = 0; i < model_part_num; ++i)
	{
		std::string model_vertex_data_name = path_name.str() + file_name.str() + std::to_string(i) + ".vertex";
		std::string model_index_data_name = path_name.str() + file_name.str() + std::to_string(i) + ".index";
		if (if_skinmesh)
		{
			check_error = LoadMeshData<PointSkinCommon8>(model_vertex_data_name, model_index_data_name);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		else if (if_pointmesh)
		{
			check_error = LoadMeshData<PointCatchCommon>(model_vertex_data_name, model_index_data_name);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
		else
		{
			check_error = LoadMeshData<PointCommon>(model_vertex_data_name, model_index_data_name);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
		}
	}
	//读取骨骼动画
	if (if_skinmesh)
	{
		std::string bone_data_name = path_name.str() + file_name.str() + ".bone";
		//读取骨骼信息
		check_error = LoadSkinTree(bone_data_name);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//读取动画信息
		Json::Value skin_animation_value = root_value.get("SkinAnimation", Json::Value::null);
		for (Json::ArrayIndex i = 0; i < skin_animation_value.size(); ++i)
		{
			animation_set new_animation;
			check_error = PancyJsonTool::GetInstance()->GetJsonData(resource_name, skin_animation_value, i, pancy_json_data_type::json_data_string, rec_value);
			if (!check_error.m_IsOK)
			{
				return check_error;
			}
			std::string now_animation_name = path_name.str() + rec_value.string_value;
			instream.open(now_animation_name, ios::binary);
			int32_t animation_bone_num;
			float animation_length;
			instream.read(reinterpret_cast<char*>(&animation_length), sizeof(animation_length));
			instream.read(reinterpret_cast<char*>(&animation_bone_num), sizeof(animation_bone_num));
			new_animation.animation_length = animation_length;
			for (int l = 0; l < animation_bone_num; ++l)
			{
				animation_data new_bone_data;
				//骨骼信息
				int32_t bone_name_size = -1;
				instream.read(reinterpret_cast<char*>(&bone_name_size), sizeof(bone_name_size));
				char* name = new char[bone_name_size];
				instream.read(name, bone_name_size * sizeof(char));
				new_bone_data.bone_name += name;
				auto now_used_bone_ID = bone_name_index.find(new_bone_data.bone_name);
				if (now_used_bone_ID == bone_name_index.end())
				{
					LResult error_message;
					LunarDebugLogError(E_FAIL, "could not find bone: " + new_bone_data.bone_name + " in model: " + resource_name, error_message);

					return error_message;
				}
				new_bone_data.bone_ID = now_used_bone_ID->second;
				delete[] name;
				//旋转向量
				int32_t rotation_key_num = 0;
				instream.read(reinterpret_cast<char*>(&rotation_key_num), sizeof(rotation_key_num));
				quaternion_animation* new_rotation_key = new quaternion_animation[rotation_key_num];
				int32_t rotation_key_size = sizeof(new_rotation_key[0]) * rotation_key_num;

				instream.read(reinterpret_cast<char*>(new_rotation_key), rotation_key_size);
				for (int j = 0; j < rotation_key_num; ++j)
				{
					new_bone_data.rotation_key.push_back(new_rotation_key[j]);
				}
				//平移向量
				int32_t translation_key_num = 0;
				instream.read(reinterpret_cast<char*>(&translation_key_num), sizeof(translation_key_num));
				vector_animation* new_translation_key = new vector_animation[translation_key_num];
				int32_t translation_key_size = sizeof(new_translation_key[0]) * translation_key_num;
				instream.read(reinterpret_cast<char*>(new_translation_key), translation_key_size);
				for (int j = 0; j < translation_key_num; ++j)
				{
					new_bone_data.translation_key.push_back(new_translation_key[j]);
				}

				//缩放向量
				int32_t scaling_key_num = 0;
				instream.read(reinterpret_cast<char*>(&scaling_key_num), sizeof(scaling_key_num));
				vector_animation* new_scaling_key = new vector_animation[scaling_key_num];
				int32_t scaling_key_size = sizeof(new_scaling_key[0]) * scaling_key_num;
				instream.read(reinterpret_cast<char*>(new_scaling_key), scaling_key_size);
				for (int j = 0; j < scaling_key_num; ++j)
				{
					new_bone_data.scaling_key.push_back(new_scaling_key[j]);
				}
				new_animation.data_animition.push_back(new_bone_data);
				//删除临时变量
				delete[] new_rotation_key;
				delete[] new_translation_key;
				delete[] new_scaling_key;
			}
			//将动画信息加入表单
			skin_animation_name.insert(std::pair<std::string, LunarResourceID>(now_animation_name, i));
			skin_animation_map.insert(std::pair<LunarResourceID, animation_set>(i, new_animation));
			instream.close();
		}
		//将所有的动画数据组织成一个统一的buffer
		std::vector<AnimationNodeData> animation_buffer_data;
		for (auto& skin_anim_data : skin_animation_map)
		{
			int32_t max_resample_data = 0;
			AnimationNodeData identity_animation_data;
			identity_animation_data.rotation_key.x = 0;
			identity_animation_data.rotation_key.y = 0;
			identity_animation_data.rotation_key.z = 0;
			identity_animation_data.rotation_key.w = 1;
			identity_animation_data.scaling_key.x = 1;
			identity_animation_data.scaling_key.y = 1;
			identity_animation_data.scaling_key.z = 1;
			identity_animation_data.scaling_key.w = 0;
			identity_animation_data.translation_key.x = 0;
			identity_animation_data.translation_key.y = 0;
			identity_animation_data.translation_key.z = 0;
			identity_animation_data.translation_key.w = 0;
			LunarObjectID now_animation_begin = static_cast<LunarObjectID>(animation_buffer_data.size());
			animation_start_offset.push_back(now_animation_begin * 3);
			for (auto each_bone_data_anim : skin_anim_data.second.data_animition)
			{
				if (static_cast<int32_t>(each_bone_data_anim.rotation_key.size()) > max_resample_data)
				{
					max_resample_data = static_cast<int32_t>(each_bone_data_anim.rotation_key.size());
				}
			}
			for (int32_t bone_id_use = 0; bone_id_use < static_cast<int32_t>(GetBoneNum() + 1); ++bone_id_use)
			{
				for (int32_t sample_id_use = 0; sample_id_use < max_resample_data; ++sample_id_use)
				{
					animation_buffer_data.push_back(identity_animation_data);
				}
			}

			for (auto each_bone_data_anim : skin_anim_data.second.data_animition)
			{
				LunarObjectID now_bone_id = each_bone_data_anim.bone_ID;
				LunarObjectID now_begin_id = now_animation_begin + (now_bone_id + 1) * max_resample_data;
				if (each_bone_data_anim.translation_key.size() == each_bone_data_anim.rotation_key.size() && each_bone_data_anim.rotation_key.size() == each_bone_data_anim.scaling_key.size())
				{
					if (max_resample_data == 0 || max_resample_data == each_bone_data_anim.translation_key.size())
					{
						for (int32_t key_index = 0; key_index < max_resample_data; ++key_index)
						{
							AnimationNodeData& new_animation_data = animation_buffer_data[now_begin_id + key_index];
							new_animation_data.translation_key.x = each_bone_data_anim.translation_key[key_index].main_key[0];
							new_animation_data.translation_key.y = each_bone_data_anim.translation_key[key_index].main_key[1];
							new_animation_data.translation_key.z = each_bone_data_anim.translation_key[key_index].main_key[2];

							new_animation_data.rotation_key.x = each_bone_data_anim.rotation_key[key_index].main_key[0];
							new_animation_data.rotation_key.y = each_bone_data_anim.rotation_key[key_index].main_key[1];
							new_animation_data.rotation_key.z = each_bone_data_anim.rotation_key[key_index].main_key[2];
							new_animation_data.rotation_key.w = each_bone_data_anim.rotation_key[key_index].main_key[3];

							new_animation_data.scaling_key.x = each_bone_data_anim.scaling_key[key_index].main_key[0];
							new_animation_data.scaling_key.y = each_bone_data_anim.scaling_key[key_index].main_key[1];
							new_animation_data.scaling_key.z = each_bone_data_anim.scaling_key[key_index].main_key[2];
						}
					}
					else
					{
						//todo:其中一个骨骼的采样率和其他骨骼不一样
						if (each_bone_data_anim.translation_key.size() == 1)
						{
							for (int32_t key_index = 0; key_index < max_resample_data; ++key_index)
							{
								AnimationNodeData& new_animation_data = animation_buffer_data[now_begin_id + key_index];
								new_animation_data.translation_key.x = each_bone_data_anim.translation_key[0].main_key[0];
								new_animation_data.translation_key.y = each_bone_data_anim.translation_key[0].main_key[1];
								new_animation_data.translation_key.z = each_bone_data_anim.translation_key[0].main_key[2];

								new_animation_data.rotation_key.x = each_bone_data_anim.rotation_key[0].main_key[0];
								new_animation_data.rotation_key.y = each_bone_data_anim.rotation_key[0].main_key[1];
								new_animation_data.rotation_key.z = each_bone_data_anim.rotation_key[0].main_key[2];
								new_animation_data.rotation_key.w = each_bone_data_anim.rotation_key[0].main_key[3];

								new_animation_data.scaling_key.x = each_bone_data_anim.scaling_key[0].main_key[0];
								new_animation_data.scaling_key.y = each_bone_data_anim.scaling_key[0].main_key[1];
								new_animation_data.scaling_key.z = each_bone_data_anim.scaling_key[0].main_key[2];
							}
						}
						else
						{
							LResult error_message;
							LunarDebugLogError(E_FAIL, "could not resample animation", error_message);

							return error_message;
						}
					}
				}
				else
				{
					int32_t now_resample = static_cast<int32_t>(max(each_bone_data_anim.translation_key.size(), each_bone_data_anim.rotation_key.size()));
					now_resample = static_cast<int32_t>(max(now_resample, each_bone_data_anim.scaling_key.size()));
					if (max_resample_data != 0 && now_resample != max_resample_data)
					{
						LResult error_message;
						LunarDebugLogError(E_FAIL, "could not resample animation", error_message);

						return error_message;
					}
					for (int32_t key_index = 0; key_index < max_resample_data; ++key_index)
					{
						AnimationNodeData& new_animation_data = animation_buffer_data[now_begin_id + key_index];
						if (each_bone_data_anim.translation_key.size() == now_resample)
						{
							new_animation_data.translation_key.x = each_bone_data_anim.translation_key[key_index].main_key[0];
							new_animation_data.translation_key.y = each_bone_data_anim.translation_key[key_index].main_key[1];
							new_animation_data.translation_key.z = each_bone_data_anim.translation_key[key_index].main_key[2];
						}
						else if (each_bone_data_anim.translation_key.size() == 1)
						{
							new_animation_data.translation_key.x = each_bone_data_anim.translation_key[0].main_key[0];
							new_animation_data.translation_key.y = each_bone_data_anim.translation_key[0].main_key[1];
							new_animation_data.translation_key.z = each_bone_data_anim.translation_key[0].main_key[2];
						}
						else
						{
							LResult error_message;
							LunarDebugLogError(E_FAIL, "could not resample animation", error_message);

							return error_message;
						}

						if (each_bone_data_anim.rotation_key.size() == now_resample)
						{
							new_animation_data.rotation_key.x = each_bone_data_anim.rotation_key[key_index].main_key[0];
							new_animation_data.rotation_key.y = each_bone_data_anim.rotation_key[key_index].main_key[1];
							new_animation_data.rotation_key.z = each_bone_data_anim.rotation_key[key_index].main_key[2];
							new_animation_data.rotation_key.w = each_bone_data_anim.rotation_key[key_index].main_key[3];
						}
						else if (each_bone_data_anim.rotation_key.size() == 1)
						{
							new_animation_data.rotation_key.x = each_bone_data_anim.rotation_key[0].main_key[0];
							new_animation_data.rotation_key.y = each_bone_data_anim.rotation_key[0].main_key[1];
							new_animation_data.rotation_key.z = each_bone_data_anim.rotation_key[0].main_key[2];
							new_animation_data.rotation_key.w = each_bone_data_anim.rotation_key[0].main_key[3];
						}
						else
						{
							LResult error_message;
							LunarDebugLogError(E_FAIL, "could not resample animation", error_message);

							return error_message;
						}

						if (each_bone_data_anim.scaling_key.size() == now_resample)
						{
							new_animation_data.scaling_key.x = each_bone_data_anim.scaling_key[key_index].main_key[0];
							new_animation_data.scaling_key.y = each_bone_data_anim.scaling_key[key_index].main_key[1];
							new_animation_data.scaling_key.z = each_bone_data_anim.scaling_key[key_index].main_key[2];
						}
						else if (each_bone_data_anim.scaling_key.size() == 1)
						{
							new_animation_data.scaling_key.x = each_bone_data_anim.scaling_key[0].main_key[0];
							new_animation_data.scaling_key.y = each_bone_data_anim.scaling_key[0].main_key[1];
							new_animation_data.scaling_key.z = each_bone_data_anim.scaling_key[0].main_key[2];
						}
						else
						{
							LResult error_message;
							LunarDebugLogError(E_FAIL, "could not resample animation", error_message);

							return error_message;
						}
					}
				}
			}
			animation_resample_size.push_back(max_resample_data);
		}
		//创建存储动画数据的缓冲区资源(静态缓冲区)
		auto real_buffer_size = animation_buffer_data.size() * sizeof(AnimationNodeData);
		animation_buffer_size = 3 * animation_buffer_data.size();
		check_error = BuildBufferResourceFromMemory(file_name.str() + "_animation", model_animation_buffer, &animation_buffer_data[0], real_buffer_size, true);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//创建骨骼数据的父节点缓冲区资源（注意这里需要添加两个额外的骨骼即头骨骼与尾骨骼。这样会方便Computeshader进行并行处理）
		std::vector<uint32_t> bone_node_parent_data;
		bone_node_parent_data.resize(bone_parent_data.size() + 2);
		for (auto now_bone_node : bone_parent_data)
		{
			bone_node_parent_data[static_cast<size_t>(now_bone_node.first) + static_cast<size_t>(1)] = now_bone_node.second;
		}
		std::vector<uint32_t> bone_node_parent_multilayer_data;
		bone_node_parent_multilayer_data.resize(MaxSkinDeSampleTime * bone_node_parent_data.size());
		int32_t pow2_save[] = { 1,2,4,8,16,32,64,128,256 };
		for (uint32_t now_node_index = 0; now_node_index < bone_node_parent_data.size(); ++now_node_index)
		{
			for (int32_t layer_index = 1; layer_index < MaxSkinDeSampleTime; ++layer_index)
			{
				int32_t globle_index = MaxSkinDeSampleTime * now_node_index + layer_index - 1;
				int32_t now_index_search = pow2_save[layer_index - 1];
				bone_node_parent_multilayer_data[globle_index] = FindParentByLayer(now_node_index, now_index_search, bone_node_parent_data);
			}
		}
		//创建存储父骨骼数据的缓冲区资源(静态缓冲区)
		auto real_parent_buffer_size = bone_node_parent_multilayer_data.size() * sizeof(uint32_t);
		bonetree_buffer_size = bone_node_parent_multilayer_data.size();
		check_error = BuildBufferResourceFromMemory(file_name.str() + "_boneparent", model_bonetree_buffer, &bone_node_parent_multilayer_data[0], real_parent_buffer_size, true);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		//创建偏移矩阵缓冲区资源(同样需要头骨骼与尾骨骼)
		std::vector<DirectX::XMFLOAT4X4> bone_offset_mat_data;
		DirectX::XMFLOAT4X4 identity_mat;
		DirectX::XMStoreFloat4x4(&identity_mat, DirectX::XMMatrixIdentity());
		bone_offset_mat_data.push_back(identity_mat);
		for (int bone_id = 0; bone_id < bone_object_num; ++bone_id)
		{
			bone_offset_mat_data.push_back(identity_mat);
		}
		bone_offset_mat_data.push_back(identity_mat);
		for (int bone_id = 0; bone_id < bone_num; ++bone_id)
		{
			bone_offset_mat_data[bone_id + 1] = offset_matrix_array[bone_id];
		}
		//创建存储偏移矩阵数据的缓冲区资源(静态缓冲区)
		auto real_offset_buffer_size = bone_offset_mat_data.size() * sizeof(DirectX::XMFLOAT4X4);
		boneoffset_buffer_size = bone_offset_mat_data.size();
		check_error = BuildBufferResourceFromMemory(file_name.str() + "_boneoffset", model_boneoffset_buffer, &bone_offset_mat_data[0], real_offset_buffer_size, true);
		if (!check_error.m_IsOK)
		{
			return check_error;
		}
		int a = 0;
	}


	//读取顶点动画
	/*
	if (if_pointmesh)
	{
		Json::Value point_animation_value = root_value.get("PointAnimation", Json::Value::null);
		for (int i = 0; i < point_animation_value.size(); ++i)
		{
			check_error = PancyJsonTool::GetInstance()->GetJsonData(resource_name, point_animation_value, i, pancy_json_data_type::json_data_string, rec_value);
			if (!check_error.if_succeed)
			{
				return check_error;
			}
			std::string now_animation_name = path_name + rec_value.string_value;
			instream.open(now_animation_name, ios::binary);
			instream.read(reinterpret_cast<char*>(&all_frame_num), sizeof(all_frame_num));
			instream.read(reinterpret_cast<char*>(&mesh_animation_buffer_size), sizeof(mesh_animation_buffer_size));
			instream.read(reinterpret_cast<char*>(&mesh_animation_ID_buffer_size), sizeof(mesh_animation_ID_buffer_size));
			int32_t buffer_size = mesh_animation_buffer_size * sizeof(mesh_animation_data);
			int32_t buffer_id_size = mesh_animation_ID_buffer_size * sizeof(VertexAnimationID);
			mesh_animation_data *new_point_catch_data = new mesh_animation_data[buffer_size];
			instream.read(reinterpret_cast<char*>(new_point_catch_data), buffer_size);
			VertexAnimationID *new_point_id_data = new VertexAnimationID[buffer_id_size];
			instream.read(reinterpret_cast<char*>(new_point_id_data), buffer_id_size);
			instream.close();
			LunarObjectID point_aimation_buffer_pointer;
			LunarObjectID point_aimation_id_buffer_pointer;
			Json::Value point_animation_buffer_type;
			std::string buffer_subresource_name;
			check_error = PancyBasicBufferControl::GetInstance()->BuildBufferTypeJson(Buffer_ShaderResource_static, buffer_size, buffer_subresource_name);
			if (!check_error.if_succeed)
			{
				return check_error;
			}
			PancyJsonTool::GetInstance()->SetJsonValue(point_animation_buffer_type, "BufferType", "Buffer_ShaderResource_static");
			PancyJsonTool::GetInstance()->SetJsonValue(point_animation_buffer_type, "SubResourceFile", buffer_subresource_name);
			check_error = PancyBasicBufferControl::GetInstance()->LoadResource(file_name, point_animation_buffer_type, point_aimation_buffer_pointer, true);
			if (!check_error.if_succeed)
			{
				return check_error;
			}

			Json::Value point_animation_buffer_id_type;
			check_error = PancyBasicBufferControl::GetInstance()->BuildBufferTypeJson(Buffer_ShaderResource_static, buffer_id_size, buffer_subresource_name);
			if (!check_error.if_succeed)
			{
				return check_error;
			}
			PancyJsonTool::GetInstance()->SetJsonValue(point_animation_buffer_id_type, "BufferType", "Buffer_ShaderResource_static");
			PancyJsonTool::GetInstance()->SetJsonValue(point_animation_buffer_id_type, "SubResourceFile", buffer_subresource_name);
			check_error = PancyBasicBufferControl::GetInstance()->LoadResource(file_name, point_animation_buffer_id_type, point_aimation_id_buffer_pointer, true);
			if (!check_error.if_succeed)
			{
				return check_error;
			}
			delete[] new_point_catch_data;
			delete[] new_point_id_data;
			Point_animation_name.insert(std::pair<std::string, LunarObjectID>(now_animation_name, point_animation_id_self_add));
			Point_animation_buffer.insert(std::pair<LunarObjectID, LunarObjectID>(point_animation_id_self_add, point_aimation_buffer_pointer));
			Point_animation_id_buffer.insert(std::pair<LunarObjectID, LunarObjectID>(point_animation_id_self_add, point_aimation_id_buffer_pointer));
			point_animation_id_self_add += 1;
		}
	}
	*/
	return g_Succeed;
}
bool PancyBasicModel::CheckIfLoadSucceed()
{
	//检测所有的几何体资源是否已经加载完毕
	for (int i = 0; i < model_resource_list.size(); ++i)
	{
		bool if_mesh_succeed_load = model_resource_list[i]->CheckIfLoadSucceed();
		if (!if_mesh_succeed_load)
		{
			return false;
		}
	}
	return true;
}
