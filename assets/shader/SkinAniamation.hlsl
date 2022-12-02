#include"matrix.hlsl"
#define threadBlockSize 64
cbuffer animation_sample_param : register(b0)
{
	uint4 animation_id_param;    //{模型的id，骨骼数量,动画重采样数量,动画的总帧数}
	uint4 animation_begin_param; //{骨骼缓冲区的起始点，动画缓冲区起始点}
	float4 animation_play_param; //{动画的播放时间}
}
RWStructuredBuffer<float4x4> bone_matrix_output : register(u0);	//采样得到的骨骼数据
RWStructuredBuffer<uint4>    compute_globel_id_input    : register(u1);	//全局id记录
StructuredBuffer<float4>     animation_data[]     : register(t0);	//动画数据

[numthreads(threadBlockSize, 1, 1)]
void CSMain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	//计算当前的骨骼的id号，如果超过了最大骨骼数量，则认为是尾骨骼
	uint now_bone_size = animation_id_param.y;
	uint now_thread_id = groupId.x * threadBlockSize + groupIndex;
	uint max_thread_id = animation_id_param.y - 1;
	uint now_bone_index = min(now_thread_id, max_thread_id);
	uint model_id = animation_id_param.x;
	uint animation_resample_num = animation_id_param.z;
	//当前的动画帧对应的骨骼数据的起始位置(动画数据在全局buffer的位置 + 当前骨骼的动画数据在所有骨骼动画数据的位置)
	uint now_bone_animation_offset = animation_begin_param.y + now_bone_index * 3 * animation_resample_num;
	//根据动画时间，获取需要插值的两帧动画数据(这一句感觉可以优化，由cpu计算)
	float now_real_animation_pos = animation_play_param.x * (float)(animation_resample_num - 1);
	uint now_animation_play_scal_st = floor(now_real_animation_pos);
	uint now_animation_play_scal_ed = ceil(now_real_animation_pos);

	uint now_animation_sample_st = now_bone_animation_offset + now_animation_play_scal_st * 3;
	uint now_animation_sample_ed = now_bone_animation_offset + now_animation_play_scal_ed * 3;

	float lerp_length = max(float(now_animation_play_scal_ed) - float(now_animation_play_scal_st), 0.00001f);
	float lerp_value = max(now_real_animation_pos - float(now_animation_play_scal_st), 0.0f);
	float lerp_delta = saturate(lerp_value/ lerp_length);
	//线性插值平移与缩放向量
	float4 translation_lerp = lerp(animation_data[model_id][now_animation_sample_st], animation_data[model_id][now_animation_sample_ed], lerp_delta);
	float4 scal_lerp = lerp(animation_data[model_id][now_animation_sample_st + 2], animation_data[model_id][now_animation_sample_ed + 2], lerp_delta);
	//插值旋转四元数
	float4 rotation_quaternion_begin = animation_data[model_id][now_animation_sample_st + 1];
	float4 rotation_quaternion_end = animation_data[model_id][now_animation_sample_ed + 1];
	//根据点积结果正负，判断四元数是否同向
	float cosom = dot(rotation_quaternion_begin, rotation_quaternion_end);
	float if_change = sign(cosom);
	cosom *= if_change;
	rotation_quaternion_end = rotation_quaternion_end * if_change;
	//根据将点积结果限制在0.99使用球面插值
	float sclp, sclq;
	float real_count_cosom = min(cosom, 0.99999);
	float omega, sinom;
	omega = acos(real_count_cosom);
	sinom = sin(omega);
	sclp = sin((1.0f - lerp_delta) * omega) / sinom;
	sclq = sin(lerp_delta * omega) / sinom;
	float4 rotation_lerp = rotation_quaternion_begin * sclp + rotation_quaternion_end * sclq;
	//将插值后的向量及四元数转换为矩阵
	float4x4 combine_matrix = compose(translation_lerp.xyz, rotation_lerp, scal_lerp.xyz);
	uint now_animation_begin_id = animation_begin_param.x;
	uint now_bone_data_save_offset = now_animation_begin_id + now_bone_index;
	//将处理完的骨骼信息写入全局动画缓冲区
	bone_matrix_output[now_bone_data_save_offset] = combine_matrix;
	compute_globel_id_input[now_bone_data_save_offset].x = model_id;
	compute_globel_id_input[now_bone_data_save_offset].y = now_bone_index;
	compute_globel_id_input[now_bone_data_save_offset].z = now_animation_begin_id;
}