#define threadBlockSize 64
cbuffer animation_sample_param : register(b0)
{
	uint4 bone_size_param;    //{参与计算的骨骼数量}
}
StructuredBuffer<float4x4>   bone_matrix_input        : register(t0);	//骨骼树输入
StructuredBuffer<uint4>      compute_globel_id        : register(t1);	//骨骼树全局id记录
RWStructuredBuffer<float4x4> bone_matrix_output       : register(u0);	//合并骨骼矩阵输出
StructuredBuffer<float4x4>   offset_matrix[]          : register(t2);	//骨骼树父骨骼id记录

[numthreads(threadBlockSize, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
	//将骨骼id号限制在需要计算的范围，最后一个骨骼一般是尾骨骼，不会影响结果的使用
	uint now_deal_bone_id = min(DTid.x, bone_size_param.x - 1);
	//获取当前的bufferid以及骨骼id
	uint now_buffer_id = compute_globel_id[DTid.x].x;
	uint now_bone_id = compute_globel_id[DTid.x].y;
	//根据全局id获取线程对应的偏移矩阵
	float4x4 now_offset_matrix = offset_matrix[now_buffer_id][now_bone_id];
	//将当前矩阵的数据乘以父矩阵，存储在ping-pong buffer中
	bone_matrix_output[now_deal_bone_id] = transpose(mul(bone_matrix_input[now_deal_bone_id], now_offset_matrix));
}