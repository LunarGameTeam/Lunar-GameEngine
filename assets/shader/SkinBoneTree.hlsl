#define threadBlockSize 64
cbuffer animation_sample_param : register(b0)
{
	uint4 draw_pass_id_param;    //{drawpass的id，参与计算的骨骼数量}
	uint4 bone_id_param;         //{父骨骼的记录深度，}
}
StructuredBuffer<float4x4> bone_matrix_input        : register(t0);	//骨骼树输入
StructuredBuffer<uint4>    compute_globel_id        : register(t1);	//骨骼树全局id记录
RWStructuredBuffer<float4x4> bone_matrix_output     : register(u0);	//骨骼树输出
StructuredBuffer<uint>     parent_globel_id[]       : register(t2);	//骨骼树父骨骼id记录

[numthreads(threadBlockSize, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	//将骨骼id号限制在需要计算的范围，最后一个骨骼一般是尾骨骼，不会影响结果的使用
	uint now_max_bone_size = draw_pass_id_param.y - 1;
	uint now_thread_id = DTid.x;
	uint now_deal_bone_globel_id = min(now_thread_id, now_max_bone_size);
	//获取当前的bufferid以及骨骼id
	uint now_buffer_id = compute_globel_id[now_deal_bone_globel_id].x;
	uint now_bone_local_id = compute_globel_id[now_deal_bone_globel_id].y;
	uint now_matrix_begin_id = compute_globel_id[now_deal_bone_globel_id].z;
	//根据渲染passid计算当前的父骨骼id(当前骨骼数量 * 父骨骼记录深度 + 父骨骼采样drawpass id)
	uint now_parent_sample_id = bone_id_param.x * now_bone_local_id + draw_pass_id_param.x;
	//根据bufferid和父骨骼采样id得到父骨骼的全局id
	uint now_parent_bone_globel_id = now_matrix_begin_id + parent_globel_id[now_buffer_id][now_parent_sample_id];
	//根据父骨骼的全局id获取父骨骼的矩阵数据
	float4x4 now_parent_bone_matrix = bone_matrix_input[now_parent_bone_globel_id];
	float4x4 now_dealed_bone_matrix = bone_matrix_input[now_deal_bone_globel_id];
	//将当前矩阵的数据乘以父矩阵，存储在ping-pong buffer中（由于矩阵列存储，这里反向乘）
	bone_matrix_output[now_deal_bone_globel_id] = mul(now_parent_bone_matrix, now_dealed_bone_matrix);
}