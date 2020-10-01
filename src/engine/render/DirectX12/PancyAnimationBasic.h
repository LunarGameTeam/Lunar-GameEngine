#pragma once
#include"PancyBufferDx12.h"
#include"PancyRenderParam.h"
namespace LunarEngine
{
#define MaxSkinAnimationComputeNum 4096
	//骨骼动画缓冲区块
	struct SkinAnimationBlock
	{
		LunarResourceSize start_pos;
		LunarResourceSize block_size;
	};
	//顶点动画数据
	struct mesh_animation_data
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		//float delta_time;
		mesh_animation_data()
		{
			position = DirectX::XMFLOAT3(0, 0, 0);
			normal = DirectX::XMFLOAT3(0, 0, 0);
			tangent = DirectX::XMFLOAT3(0, 0, 0);
		}
	};
	//骨骼动画缓冲区
	class PancySkinAnimationBuffer
	{
		//缓冲区的大小
		LunarResourceSize animation_buffer_size;//存储蒙皮结果的缓冲区的大小
		LunarResourceSize bone_buffer_size;//存储骨骼矩阵的缓冲区的大小

		//当前已经被占用的指针位置
		LunarResourceSize now_used_position_animation;//当前动画结果缓冲区的使用情况指针
		LunarResourceSize now_used_position_bone;//当前骨骼矩阵缓冲区的使用情况指针

		//存储每一个骨骼动画的Compute Shader计算位置
		std::unordered_map<LunarObjectID, SkinAnimationBlock> animation_block_map;
		//存储每一个骨骼矩阵区域的起始位置
		std::unordered_map<LunarObjectID, SkinAnimationBlock> bone_block_map;
		//骨骼动画缓冲区的数据
		VirtualResourcePointer buffer_animation;         //动画结果缓冲区
		std::vector<VirtualResourcePointer> buffer_bone; //骨骼矩阵缓冲区
		std::vector<VirtualResourcePointer> buffer_globel_index;      //节点全局序号缓冲区

		LunarEngine::BindDescriptorPointer bone_matrix_descriptor_uav;
		LunarEngine::BindDescriptorPointer globel_id_descriptor_uav;

		LunarEngine::BindDescriptorPointer bone_matrix_descriptor_srv;
		LunarEngine::BindDescriptorPointer globel_id_descriptor_srv;
	public:
		PancySkinAnimationBuffer(const LunarResourceSize& animation_buffer_size_in, const LunarResourceSize& bone_buffer_size_in);
		~PancySkinAnimationBuffer();
		LunarEngine::LResult Create();
		//清空当前所有使用的骨骼动画数据(由于动画数据逐帧重置，不需要考虑随机寻址类型的增删查改)
		void ClearUsedBuffer();
		//从当前蒙皮结果缓冲区中请求一块数据区(蒙皮结果数据区由GPU填充数据，因而只需要开辟)
		LunarEngine::LResult BuildAnimationBlock(
			const LunarResourceSize& vertex_num,
			LunarObjectID& block_id,
			SkinAnimationBlock& new_animation_block
		);
		//从当前骨骼矩阵缓冲区中请求一块数据区(骨骼矩阵数据区由CPU填充数据，因而需要将填充数据一并传入)
		LunarEngine::LResult BuildBoneBlock(
			const LunarResourceSize& matrix_num,
			LunarObjectID& block_id,
			SkinAnimationBlock& new_bone_block
		);
		//获取矩阵存储缓冲区
		inline VirtualResourcePointer& GetBoneMatrixResource(const LunarObjectID& frame_id)
		{
			return buffer_bone[frame_id];
		}
		//获取蒙皮结果缓冲区
		inline VirtualResourcePointer& GetSkinVertexResource()
		{
			return buffer_animation;
		}
		//获取骨骼id号缓冲区
		inline VirtualResourcePointer& GetBoneIDResource()
		{
			return buffer_globel_index[0];
		}
	};
	struct AnimationStartMessage
	{
		//当前的骨骼信息
		LunarObjectID bone_matrix_offset_from_begin;
		LunarObjectID bone_matrix_num;
		//当前的父骨骼id信息
		LunarObjectID bone_parent_id_offset_from_begin;
		LunarObjectID bone_parent_id_num;
		//当前的动画信息
		LunarObjectID drawcall_id;
		LunarObjectID animation_resample_num;
		LunarObjectID animation_start_pos;//动画数据的起始点(即选择播放的是哪个动画)
		//当前的骨骼偏移信息
		LunarObjectID offset_matrix_offset_from_begin;
		LunarObjectID offset_matrix_num;
		//当前的骨骼矩阵申请情况
		LunarObjectID bone_buffer_block_id;

	};
	//全局骨骼动画控制器
	class PancySkinAnimationControl
	{
		PancyDescriptorHeapDynamic skin_mesh_animation_buffer_descriptor;
		PancyDescriptorHeapDynamic skin_mesh_offset_matrix_descriptor;
		PancyDescriptorHeapDynamic skin_mesh_parent_id_descriptor;

		LunarResourceSize animation_buffer_size;                    //存储动画结果的缓冲区大小
		LunarResourceSize bone_buffer_size;                         //存储骨骼矩阵的缓冲区大小

		LunarObjectID PSO_skinmesh_animation_interpolation;         //骨骼动画的插值阶段
		LunarObjectID PSO_skinmesh_skintree_desample;               //骨骼动画的骨骼树下采样阶段
		LunarObjectID PSO_skinmesh_cluster_combine;                 //骨骼动画的矩阵合并阶段
		//计算着色器的渲染单元id号
		LunarEngine::PancyRenderParamID render_param_id_animation_sample;
		LunarEngine::PancyRenderParamID render_param_id_bone_compute[10];
		LunarEngine::PancyRenderParamID render_param_id_combine_matrix;

		LunarObjectID PSO_skinmesh;                                 //骨骼动画的渲染状态表
		std::vector<PancySkinAnimationBuffer*> skin_naimation_buffer; //骨骼动画的缓冲区信息
		//todo:这里暂时先使用shared_ptr之后需要改成weak
		LunarObjectID bone_count_time_num = 0;
		std::vector<VirtualResourcePointer> skin_animation_matrix_buffer; //骨骼的动画矩阵缓冲区信息
		std::vector<VirtualResourcePointer> skin_offset_matrix_buffer;    //骨骼的偏移矩阵缓冲区信息
		std::vector<VirtualResourcePointer> skin_parent_id_buffer;        //骨骼的父骨骼缓冲区信息

		std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC> skin_animation_matrix_descriptor;     //骨骼动画的骨骼矩阵缓冲区信息
		std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC> skin_offset_matrix_descriptor;   //骨骼动画的偏移矩阵缓冲区信息
		std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC> skin_parent_id_descriptor;       //骨骼动画的父骨骼id缓冲区信息

		std::vector<AnimationStartMessage> skin_animation_message;              //骨骼动画的位置信息
		LunarObjectID globel_bone_matrix_size = 0;
		LunarObjectID globel_offset_matrix_size = 0;
		LunarObjectID globel_parent_id_size = 0;
		//初始化
		PancySkinAnimationControl(
			const LunarResourceSize& animation_buffer_size_in,
			const LunarResourceSize& bone_buffer_size_in
		);
		LunarEngine::LResult Create();
		//临时存储交换缓冲区描述符
		BindDescriptorPointer buffer_descriptor_srv_id1;
		BindDescriptorPointer buffer_descriptor_srv_id2;
		BindDescriptorPointer buffer_descriptor_uav_id1;
		BindDescriptorPointer buffer_descriptor_uav_id2;
	public:
		static PancySkinAnimationControl* this_instance;
		static LunarEngine::LResult SingleCreate(
			const LunarResourceSize& animation_buffer_size_in,
			const LunarResourceSize& bone_buffer_size_in
		)
		{
			if (this_instance != NULL)
			{

				LunarEngine::LResult error_message;
				LunarDebugLogError(E_FAIL, "the d3d input instance have been created before", error_message);

				return error_message;
			}
			else
			{
				this_instance = new PancySkinAnimationControl(animation_buffer_size_in, bone_buffer_size_in);
				LunarEngine::LResult check_error = this_instance->Create();
				return check_error;
			}
		}
		static PancySkinAnimationControl* GetInstance()
		{
			return this_instance;
		}
		//清空当前帧的缓冲区使用信息
		void ClearUsedBuffer();
		//计算骨骼矩阵
		LunarEngine::LResult ComputeBoneMatrix(const float& play_time);
		//填充渲染commandlist
		LunarEngine::LResult BuildCommandList(
			const LunarObjectID& bone_block_id,
			VirtualResourcePointer& mesh_buffer,
			const LunarObjectID& vertex_num,
			const PancyRenderParamID& render_param_id,
			const LunarResourceSize& matrix_num,
			SkinAnimationBlock& new_animation_block,
			PancyRenderCommandList* m_commandList_skin
		);
		//从当前骨骼矩阵缓冲区中请求一块数据区(骨骼矩阵数据区由CPU填充数据，因而需要将填充数据一并传入)
		LunarEngine::LResult BuildBoneBlock(
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
		);
		~PancySkinAnimationControl();
	private:
		//从当前蒙皮结果缓冲区中请求一块数据区(蒙皮结果数据区由GPU填充数据，因而只需要开辟)
		LunarEngine::LResult BuildAnimationBlock(
			const LunarResourceSize& vertex_num,
			LunarObjectID& block_id,
			SkinAnimationBlock& animation_block_pos
		);
		//todo:移植到描述符管理
		LunarEngine::LResult BuildGlobelBufferDescriptorUAV(
			const std::string& descriptor_name,
			std::vector<VirtualResourcePointer>& buffer_data,
			const LunarResourceSize& buffer_size,
			const LunarResourceSize& per_element_size
		);
		LunarEngine::LResult BuildGlobelBufferDescriptorSRV(
			const std::string& descriptor_name,
			std::vector<VirtualResourcePointer>& buffer_data,
			const LunarResourceSize& buffer_size,
			const LunarResourceSize& per_element_size
		);
		LunarEngine::LResult BuildPrivateBufferDescriptorUAV(
			std::vector<VirtualResourcePointer>& buffer_data,
			const LunarResourceSize& buffer_size,
			const LunarResourceSize& per_element_size,
			BindDescriptorPointer& descriptor_id
		);
		LunarEngine::LResult BuildPrivateBufferDescriptorSRV(
			std::vector<VirtualResourcePointer>& buffer_data,
			const LunarResourceSize& buffer_size,
			const LunarResourceSize& per_element_size,
			BindDescriptorPointer& descriptor_id
		);

	};
}