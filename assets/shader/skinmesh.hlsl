#define threadBlockSize 128
#define MaxBoneNum 100
struct VSInputBone
{
	float3 position     : POSITION;
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	uint4  tex_id       : TEXID;
	float4 tex_uv       : TEXUV;
	uint4  bone_id      : BONEID;
	float4 bone_weight0 : BONEWEIGHTFIR;
	float4 bone_weight1 : BONEWEIGHTSEC;
};
struct mesh_anim
{
	float3 position;
	float3 normal;
	float3 tangent;
};
cbuffer per_object : register(b0)
{
	uint4 data_offset;//{骨骼数据的初始偏移,动画数据的初始偏移}
	uint4 data_num;//{模型的顶点数量,模型的骨骼数量}
}
StructuredBuffer<VSInputBone> vertex_data : register(t0);	//用于动画蒙皮的顶点数据
StructuredBuffer<float4x4>    bone_matrix_buffer        : register(t1);	//更新的骨骼数据
RWStructuredBuffer<mesh_anim> mesh_anim_data	        : register(u0);	//计算完毕的顶点动画数据

[numthreads(threadBlockSize, 1, 1)]
void CSMain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	//根据ID号查找到当前所需处理的顶点位置(骨骼偏移 + 线程组偏移 + 线程ID偏移)
	uint now_vertex_index = groupId.x * threadBlockSize + groupIndex;
	//检测当前的ID号是否属于合理的范围(ID号超过模型顶点数量的线程置空)
	if (now_vertex_index < data_num.x)
	{
		//获取原始的骨骼变换顶点
		VSInputBone now_vertex = vertex_data[now_vertex_index];
		//获取8个对应骨骼变换矩阵的位置(骨骼偏移+instancing偏移+骨骼Id号)
		uint bone_id_mask = MaxBoneNum + 100;
		uint bone_start_index = data_offset.x + groupId.y * data_num.y;
		uint bone_id_use0 = bone_start_index + now_vertex.bone_id[0] / bone_id_mask + 1;
		uint bone_id_use1 = bone_start_index + now_vertex.bone_id[1] / bone_id_mask + 1;
		uint bone_id_use2 = bone_start_index + now_vertex.bone_id[2] / bone_id_mask + 1;
		uint bone_id_use3 = bone_start_index + now_vertex.bone_id[3] / bone_id_mask + 1;
		uint bone_id_use4 = bone_start_index + now_vertex.bone_id[0] % bone_id_mask + 1;
		uint bone_id_use5 = bone_start_index + now_vertex.bone_id[1] % bone_id_mask + 1;
		uint bone_id_use6 = bone_start_index + now_vertex.bone_id[2] % bone_id_mask + 1;
		uint bone_id_use7 = bone_start_index + now_vertex.bone_id[3] % bone_id_mask + 1;
		float3 positon_bone = float3(0.0f, 0.0f, 0.0f);
		float3 normal_bone = float3(0.0f, 0.0f, 0.0f);
		float3 tangent_bone = float3(0.0f, 0.0f, 0.0f);
		positon_bone += now_vertex.bone_weight0.x * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use0]).xyz;
		normal_bone += now_vertex.bone_weight0.x * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use0]);
		tangent_bone += now_vertex.bone_weight0.x * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use0]);

		positon_bone += now_vertex.bone_weight0.y * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use1]).xyz;
		normal_bone += now_vertex.bone_weight0.y * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use1]);
		tangent_bone += now_vertex.bone_weight0.y * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use1]);

		positon_bone += now_vertex.bone_weight0.z * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use2]).xyz;
		normal_bone += now_vertex.bone_weight0.z * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use2]);
		tangent_bone += now_vertex.bone_weight0.z * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use2]);

		positon_bone += now_vertex.bone_weight0.w * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use3]).xyz;
		normal_bone += now_vertex.bone_weight0.w * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use3]);
		tangent_bone += now_vertex.bone_weight0.w * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use3]);

		positon_bone += now_vertex.bone_weight1.x * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use4]).xyz;
		normal_bone += now_vertex.bone_weight1.x * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use4]);
		tangent_bone += now_vertex.bone_weight1.x * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use4]);

		positon_bone += now_vertex.bone_weight1.y * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use5]).xyz;
		normal_bone += now_vertex.bone_weight1.y * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use5]);
		tangent_bone += now_vertex.bone_weight1.y * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use5]);

		positon_bone += now_vertex.bone_weight1.z * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use6]).xyz;
		normal_bone += now_vertex.bone_weight1.z * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use6]);
		tangent_bone += now_vertex.bone_weight1.z * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use6]);

		positon_bone += now_vertex.bone_weight1.w * mul(float4(now_vertex.position, 1.0f), bone_matrix_buffer[bone_id_use7]).xyz;
		normal_bone += now_vertex.bone_weight1.w * mul(now_vertex.normal, (float3x3)bone_matrix_buffer[bone_id_use7]);
		tangent_bone += now_vertex.bone_weight1.w * mul(now_vertex.tangent.xyz, (float3x3)bone_matrix_buffer[bone_id_use7]);

		//根据instance以及当前的顶点位置获取存储顶点动画数据的位置
		uint vertex_result_index = data_offset.y + groupId.y * data_num.x + now_vertex_index;
		mesh_anim_data[vertex_result_index].position = positon_bone;
		mesh_anim_data[vertex_result_index].normal = normal_bone;
		mesh_anim_data[vertex_result_index].tangent = tangent_bone;
	}
}