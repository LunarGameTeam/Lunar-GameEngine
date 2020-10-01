#pragma once
#include"PancyShaderDx12.h"
namespace LunarEngine
{
	class BasicRenderParam
	{
		//渲染管线
		std::string          PSO_name;
		ID3D12PipelineState* PSO_pointer = NULL;
		ID3D12RootSignature* rootsignature = NULL;
		//描述符堆
		ID3D12DescriptorHeap* descriptor_heap_use = NULL;
		//渲染所需的描述符数量，用于判断当前的渲染单元是否已经注册完毕
		bool if_render_param_inited = false;
		LunarObjectID globel_cbuffer_num = 99999;
		LunarObjectID private_cbuffer_num = 99999;
		LunarObjectID globel_shader_resource_num = 99999;
		LunarObjectID bind_shader_resource_num = 99999;
		LunarObjectID bindless_shader_resource_num = 99999;
		//渲染所需的描述符数据
		std::unordered_map<std::string, LunarEngine::BindDescriptorPointer> globel_constant_buffer;       //全局常量缓冲区
		std::unordered_map<std::string, LunarEngine::BindDescriptorPointer> private_constant_buffer;      //私有常量缓冲区
		std::unordered_map<std::string, LunarEngine::BindDescriptorPointer> globel_shader_resource;       //全局描述符
		std::unordered_map<std::string, LunarEngine::BindDescriptorPointer> bind_shader_resource;         //私有描述符
		std::unordered_map<std::string, LunarEngine::BindlessDescriptorPointer> bindless_shader_resource; //解绑定描述符
		//渲染需要绑定的rootsignature slot
		std::unordered_map<std::string, LunarObjectID> globel_constant_buffer_root_signature_offset;    //全局常量缓冲区slot
		std::unordered_map<std::string, LunarObjectID> private_constant_buffer_root_signature_offset;   //私有常量缓冲区slot
		std::unordered_map<std::string, LunarObjectID> globel_shader_resource_root_signature_offset;    //全局描述符slot
		std::unordered_map<std::string, LunarObjectID> bind_shader_resource_root_signature_offset;      //私有描述符slot
		std::unordered_map<std::string, LunarObjectID> bindless_shader_resource_root_signature_offset;  //解绑定描述符slot
		//私有存储资源
		std::unordered_map<std::string, std::vector<PancyConstantBuffer*>> per_object_cbuffer;//每个描述符独享的cbuffer，需要自主管理这片存储区域
	public:
		BasicRenderParam();
		~BasicRenderParam();
		LunarEngine::LResult GetPsoData(ID3D12PipelineState** pso_data);
		LunarEngine::LResult SetCbufferMatrix(
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const DirectX::XMFLOAT4X4& data_in,
			const LunarResourceSize& offset
		);
		LunarEngine::LResult SetCbufferFloat4(
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const DirectX::XMFLOAT4& data_in,
			const LunarResourceSize& offset
		);
		LunarEngine::LResult SetCbufferUint4(
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const DirectX::XMUINT4& data_in,
			const LunarResourceSize& offset
		);
		LunarEngine::LResult SetCbufferStructData(
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const void* data_in,
			const LunarResourceSize& data_size,
			const LunarResourceSize& offset
		);
		LunarEngine::LResult CommonCreate(
			const std::string& PSO_name,
			const std::unordered_map<std::string, BindDescriptorPointer>& bind_shader_resource_in,
			const std::unordered_map<std::string, BindlessDescriptorPointer>& bindless_shader_resource_in
		);
		LunarEngine::LResult AddToCommandList(PancyRenderCommandList* m_commandList, const D3D12_COMMAND_LIST_TYPE& render_param_type);
	private:
		//绑定描述符到渲染管线
		LunarEngine::LResult BindDescriptorToRootsignature(
			const PancyDescriptorType& bind_descriptor_type,
			const std::unordered_map<std::string, BindDescriptorPointer>& descriptor_data,
			const std::unordered_map<std::string, LunarObjectID>& root_signature_slot_data,
			const D3D12_COMMAND_LIST_TYPE& render_param_type,
			PancyRenderCommandList* m_commandList
		);
		//绑定解绑顶描述符到渲染管线
		LunarEngine::LResult BindBindlessDescriptorToRootsignature(
			const PancyDescriptorType& bind_descriptor_type,
			const std::unordered_map<std::string, BindlessDescriptorPointer>& descriptor_data,
			const std::unordered_map<std::string, LunarObjectID>& root_signature_slot_data,
			const D3D12_COMMAND_LIST_TYPE& render_param_type,
			PancyRenderCommandList* m_commandList
		);
		//检查当前的渲染单元是否已经注册完毕
		bool CheckIfInitFinished();
	};

	struct PancyRenderParamID
	{
		//渲染状态ID号
		LunarObjectID PSO_id;
		LunarObjectID render_param_id;
	};
	class RenderParamSystem
	{
		//存储每一个pso为不同的输入参数所分配的描述符表单
		std::unordered_map<LunarObjectID, LunarObjectID> render_param_id_self_add;
		std::unordered_map<LunarObjectID, std::queue<LunarObjectID>> render_param_id_reuse_table;
		std::unordered_map<LunarObjectID, std::unordered_map<LunarObjectID, BasicRenderParam*>> render_param_table;
	private:
		RenderParamSystem();
	public:
		static RenderParamSystem* GetInstance()
		{
			static RenderParamSystem* this_instance;
			if (this_instance == NULL)
			{
				this_instance = new RenderParamSystem();
			}
			return this_instance;
		}
		~RenderParamSystem();
		LunarEngine::LResult GetCommonRenderParam(
			const std::string& PSO_name,
			const std::unordered_map<std::string, BindDescriptorPointer>& bind_shader_resource_in,
			const std::unordered_map<std::string, BindlessDescriptorPointer>& bindless_shader_resource_in,
			PancyRenderParamID& render_param_id
		);
		LunarEngine::LResult AddRenderParamToCommandList(
			const PancyRenderParamID& renderparam_id,
			PancyRenderCommandList* m_commandList,
			const D3D12_COMMAND_LIST_TYPE& render_param_type
		);
		LunarEngine::LResult GetPsoData(
			const PancyRenderParamID& renderparam_id,
			ID3D12PipelineState** pso_data
		);
		LunarEngine::LResult SetCbufferMatrix(
			const PancyRenderParamID& renderparam_id,
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const DirectX::XMFLOAT4X4& data_in,
			const LunarResourceSize& offset
		);
		LunarEngine::LResult SetCbufferFloat4(
			const PancyRenderParamID& renderparam_id,
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const DirectX::XMFLOAT4& data_in,
			const LunarResourceSize& offset
		);
		LunarEngine::LResult SetCbufferUint4(
			const PancyRenderParamID& renderparam_id,
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const DirectX::XMUINT4& data_in,
			const LunarResourceSize& offset
		);
		LunarEngine::LResult SetCbufferStructData(
			const PancyRenderParamID& renderparam_id,
			const std::string& cbuffer_name,
			const std::string& variable_name,
			const void* data_in,
			const LunarResourceSize& data_size,
			const LunarResourceSize& offset
		);
		//todo:使用引用计数删除不需要的渲染单元
		LunarEngine::LResult DeleteCommonRenderParam(PancyRenderParamID& render_param_id);
	private:
		LunarEngine::LResult GetResource(const PancyRenderParamID& renderparam_id, BasicRenderParam** data_pointer);
	};

}