#pragma once
#include"PancyDx12Basic.h"
#include"core\reflection\PancyResourceJsonReflect.h"
#include"PancyMemoryBasic.h"
#include"PancyBufferDx12.h"
#include"PancyDescriptor.h"
enum PSOType
{
	PSO_TYPE_GRAPHIC = 0,
	PSO_TYPE_COMPUTE
};
enum PancyShaderDescriptorType
{
	CbufferPrivate = 0,
	CbufferGlobel,
	SRVGlobel,
	SRVPrivate,
	SRVBindless
};
struct PancyDescriptorPSODescription
{
	std::string descriptor_name;
	LunarObjectID rootsignature_slot;
};

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
	std::vector<RootSignatureParameterDesc> root_parameter_data;
	std::vector<D3D12_STATIC_SAMPLER_DESC> static_sampler_data;
	D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE;
};
INIT_REFLECTION_CLASS(RootSignatureDesc,
	reflect_data.root_parameter_data,
	reflect_data.static_sampler_data,
	reflect_data.root_signature_flags
);

struct DescriptorTypeDesc
{
	std::string name;
	PancyShaderDescriptorType type;
};
INIT_REFLECTION_CLASS(DescriptorTypeDesc,
	reflect_data.name,
	reflect_data.type
);

struct PipelineStateDescCompute
{
	PSOType pipeline_state_type;
	std::string root_signature_file;
	std::string compute_shader_file;
	std::string compute_shader_func;
	std::vector<DescriptorTypeDesc> descriptor_type;
};
INIT_REFLECTION_CLASS(PipelineStateDescCompute,
	reflect_data.pipeline_state_type,
	reflect_data.root_signature_file,
	reflect_data.compute_shader_file,
	reflect_data.compute_shader_func,
	reflect_data.descriptor_type
);
INIT_REFLECTION_CLASS(D3D12_RENDER_TARGET_BLEND_DESC,
	reflect_data.BlendEnable,
	reflect_data.LogicOpEnable,
	reflect_data.SrcBlend,
	reflect_data.DestBlend,
	reflect_data.BlendOp,
	reflect_data.SrcBlendAlpha,
	reflect_data.DestBlendAlpha,
	reflect_data.BlendOpAlpha,
	reflect_data.LogicOp,
	reflect_data.RenderTargetWriteMask
);


struct PipelineStateDescGraphic
{
	PSOType pipeline_state_type;
	std::string root_signature_file;
	std::string vertex_shader_file;
	std::string vertex_shader_func;
	std::string pixel_shader_file;
	std::string pixel_shader_func;
	std::string geometry_shader_file;
	std::string geometry_shader_func;
	std::string hull_shader_file;
	std::string hull_shader_func;
	std::string domin_shader_file;
	std::string domin_shader_func;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
	std::vector<DescriptorTypeDesc> descriptor_type;
};
INIT_REFLECTION_CLASS_WITH_STATIC_ARRAY(PipelineStateDescGraphic,
	reflect_data.pipeline_state_type,
	reflect_data.root_signature_file,
	reflect_data.vertex_shader_file,
	reflect_data.vertex_shader_func,
	reflect_data.pixel_shader_file,
	reflect_data.pixel_shader_func,
	reflect_data.geometry_shader_file,
	reflect_data.geometry_shader_func,
	reflect_data.hull_shader_file,
	reflect_data.hull_shader_func,
	reflect_data.domin_shader_file,
	reflect_data.domin_shader_func,
	reflect_data.pso_desc.RasterizerState.CullMode,
	reflect_data.pso_desc.RasterizerState.FillMode,
	reflect_data.pso_desc.BlendState.AlphaToCoverageEnable,
	reflect_data.pso_desc.BlendState.IndependentBlendEnable,
	reflect_data.pso_desc.BlendState.RenderTarget,
	reflect_data.pso_desc.DepthStencilState.DepthEnable,
	reflect_data.pso_desc.DepthStencilState.DepthWriteMask,
	reflect_data.pso_desc.DepthStencilState.DepthFunc,
	reflect_data.pso_desc.DepthStencilState.StencilEnable,
	reflect_data.pso_desc.DepthStencilState.StencilReadMask,
	reflect_data.pso_desc.DepthStencilState.StencilWriteMask,
	reflect_data.pso_desc.DepthStencilState.FrontFace.StencilFunc,
	reflect_data.pso_desc.DepthStencilState.FrontFace.StencilDepthFailOp,
	reflect_data.pso_desc.DepthStencilState.FrontFace.StencilPassOp,
	reflect_data.pso_desc.DepthStencilState.FrontFace.StencilFailOp,
	reflect_data.pso_desc.DepthStencilState.BackFace.StencilFunc,
	reflect_data.pso_desc.DepthStencilState.BackFace.StencilDepthFailOp,
	reflect_data.pso_desc.DepthStencilState.BackFace.StencilPassOp,
	reflect_data.pso_desc.DepthStencilState.BackFace.StencilFailOp,
	reflect_data.pso_desc.SampleMask,
	reflect_data.pso_desc.PrimitiveTopologyType,
	reflect_data.pso_desc.NumRenderTargets,
	reflect_data.pso_desc.RTVFormats,
	reflect_data.pso_desc.DSVFormat,
	reflect_data.pso_desc.SampleDesc.Count,
	reflect_data.pso_desc.SampleDesc.Quality,
	reflect_data.descriptor_type
);
//几何体的格式对接类型
struct PancyVertexBufferDesc
{
	std::string vertex_desc_name;
	size_t input_element_num;
	D3D12_INPUT_ELEMENT_DESC* inputElementDescs = NULL;
};
//常量缓冲区
//常量缓冲区
struct CbufferVariable
{
	LunarResourceSize variable_size;
	LunarResourceSize start_offset;
};
//todo:这里之后需要将cbuffer纳入resource管理
class PancyConstantBuffer
{
	bool if_loaded = false;
	std::string cbuffer_name;       //常量缓冲区的名称
	std::string cbuffer_effect_name; //创建常量缓冲区的渲染管线名称
	//常量缓冲区的数据
	LunarEngine::VirtualResourcePointer buffer_ID;
	LunarObjectID buffer_offset_id;
	LunarResourceSize cbuffer_size;
	//所有成员变量的起始位置
	std::unordered_map<std::string, CbufferVariable> member_variable;
	//常量缓冲区在CPU端的指针
	UINT8* map_pointer_out;
public:
	PancyConstantBuffer();
	LunarEngine::LResult Create(
		const std::string& cbuffer_name_in,
		const std::string& cbuffer_effect_name_in,
		const LunarEngine::VirtualResourcePointer& buffer_id_in,
		const LunarResourceSize& buffer_offset_id_in,
		const LunarResourceSize& cbuffer_size,
		const Json::Value& root_value
	);
	inline LunarEngine::VirtualResourcePointer& GetBufferResource()
	{
		return buffer_ID;
	}
	inline LunarResourceSize GetCbufferOffsetFromBufferHead()
	{
		return static_cast<LunarResourceSize>(buffer_offset_id) * cbuffer_size;
	}
	inline LunarResourceSize GetCbufferSize()
	{
		return cbuffer_size;
	}
	~PancyConstantBuffer();
	LunarEngine::LResult SetMatrix(const std::string& variable, const DirectX::XMFLOAT4X4& mat_data, const LunarResourceSize& offset);
	LunarEngine::LResult SetFloat4(const std::string& variable, const DirectX::XMFLOAT4& vector_data, const LunarResourceSize& offset);
	LunarEngine::LResult SetUint4(const std::string& variable, const DirectX::XMUINT4& vector_data, const LunarResourceSize& offset);
	LunarEngine::LResult SetStruct(const std::string& variable, const void* struct_data, const LunarResourceSize& data_size, const LunarResourceSize& offset);
private:
	LunarEngine::LResult GetCbufferDesc(const std::string& file_name, const Json::Value& root_value);
	LunarEngine::LResult ErrorVariableNotFind(const std::string& variable_name);
};
struct CbufferPackList
{
	LunarEngine::VirtualResourcePointer buffer_pointer;
	LunarResourceSize per_cbuffer_size;
	std::unordered_set<LunarObjectID> now_use_offset;
	std::unordered_set<LunarObjectID> now_empty_offset;
	CbufferPackList(LunarEngine::VirtualResourcePointer& buffer_data, const LunarResourceSize& per_cbuffer_size_in)
	{
		buffer_pointer = buffer_data;
		if (per_cbuffer_size_in % 256 != 0)
		{
			per_cbuffer_size = ((per_cbuffer_size_in / 256) + 1) * 256;
		}
		else
		{
			per_cbuffer_size = per_cbuffer_size_in;
		}
		const LunarEngine::PancyBasicBuffer* pointer = dynamic_cast<const LunarEngine::PancyBasicBuffer*>(buffer_pointer.GetResourceData());
		LunarObjectID all_member_cbuffer_num = static_cast<LunarObjectID>(pointer->GetBufferSize() / per_cbuffer_size);
		for (LunarObjectID index_offset = 0; index_offset < all_member_cbuffer_num; ++index_offset)
		{
			now_empty_offset.insert(index_offset);
		}
	}
};
class ConstantBufferAlloctor
{
	LunarResourceSize cbuffer_size;       //每个常量缓冲区的大小
	std::string cbuffer_name;               //常量缓冲区的名称
	std::string cbuffer_effect_name;        //创建常量缓冲区的渲染管线名称
	LunarEngine::PancyCommonBufferDesc buffer_resource_desc_value;
	//Json::Value buffer_resource_desc_value; //buffer资源格式
	Json::Value cbuffer_desc_value;         //cbuffer格式
	std::unordered_map<LunarObjectID, CbufferPackList*> all_cbuffer_list;
public:
	ConstantBufferAlloctor(
		const LunarResourceSize& cbuffer_size_in,
		const std::string& cbuffer_name_in,
		const std::string& cbuffer_effect_name_in,
		const LunarEngine::PancyCommonBufferDesc& buffer_resource_desc_value_in,
		Json::Value& cbuffer_desc_value_in
	);
	LunarEngine::LResult BuildNewCbuffer(PancyConstantBuffer& cbuffer_data);
	LunarEngine::LResult ReleaseCbuffer(const LunarObjectID& buffer_resource_id, const LunarObjectID& buffer_offset_id);
};
//几何体格式管理器(用于注册顶点)
class InputLayoutDesc
{
	std::unordered_map<std::string, PancyVertexBufferDesc> vertex_buffer_desc_map;
private:
	InputLayoutDesc();
public:
	static InputLayoutDesc* GetInstance()
	{
		static InputLayoutDesc* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new InputLayoutDesc();
		}
		return this_instance;
	}
	~InputLayoutDesc();
	void AddVertexDesc(std::string vertex_desc_name_in, std::vector<D3D12_INPUT_ELEMENT_DESC> input_element_desc_list);
	inline const PancyVertexBufferDesc* GetVertexDesc(std::string vertex_desc_name_in)
	{
		auto new_vertex_desc = vertex_buffer_desc_map.find(vertex_desc_name_in);
		if (new_vertex_desc != vertex_buffer_desc_map.end())
		{
			return &new_vertex_desc->second;
		}
		return NULL;
	}
};
class PancyShaderBasic
{
	LunarEngine::LString shader_file_name;
	LunarEngine::LString shader_entry_point_name;
	LunarEngine::LString shader_type_name;
	ComPtr<ID3DBlob> shader_memory_pointer;
	ComPtr<ID3D12ShaderReflection> shader_reflection;
	//std::unordered_map<std::string, D3D12_SHADER_BUFFER_DESC> Cbuffer_map;
public:
	PancyShaderBasic
	(
		const std::string& shader_file_in,
		const std::string& main_func_name,
		const std::string& shader_type
	);
	ComPtr<ID3DBlob> GetShader()
	{
		return shader_memory_pointer;
	}
	ComPtr<ID3D12ShaderReflection> GetShaderReflect()
	{
		return shader_reflection;
	}
	LunarEngine::LResult Create();
};
class PancyShaderControl
{
	std::unordered_map<std::string, PancyShaderBasic*> shader_list;
	PancyShaderControl();
public:
	static PancyShaderControl* GetInstance()
	{
		static PancyShaderControl* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new PancyShaderControl();
		}
		return this_instance;
	}
	~PancyShaderControl();
	LunarEngine::LResult LoadShader(const std::string& shader_file, const std::string& shader_main_func, const std::string& shader_type);
	LunarEngine::LResult GetShaderReflection(const std::string& shader_file, const std::string& shader_main_func, const std::string& shader_type, ComPtr<ID3D12ShaderReflection>* res_data);
	LunarEngine::LResult GetShaderData(const std::string& shader_file, const std::string& shader_main_func, const std::string& shader_type, ComPtr<ID3DBlob>* res_data);
};

//RootSignature
class PancyRootSignature
{
	std::string descriptor_heap_name;
	LunarEngine::LString root_signature_name;
	ComPtr<ID3D12RootSignature> root_signature_data;
public:
	PancyRootSignature(const std::string& file_name);
	LunarEngine::LResult Create();
	inline void GetResource(ID3D12RootSignature** root_signature_data_out)
	{
		*root_signature_data_out = root_signature_data.Get();
	};
private:
	LunarEngine::LResult BuildResource(const CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC& rootSignatureDesc);
	std::string GetJsonFileRealName(const std::string& file_name_in);
};

//RootSignature管理器
class PancyRootSignatureControl
{
	//空闲的RootSignature ID号
	LunarObjectID RootSig_id_self_add;
	std::queue<LunarObjectID> empty_RootSig_id;
	//RootSignature的存储结构
	std::unordered_map<std::string, LunarObjectID> RootSig_name;
	std::unordered_map<LunarObjectID, std::string> RootSig_name_reflect;
	std::unordered_map<LunarObjectID, PancyRootSignature*> root_signature_array;
private:
	PancyRootSignatureControl();
public:
	static PancyRootSignatureControl* GetInstance()
	{
		static PancyRootSignatureControl* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new PancyRootSignatureControl();
		}
		return this_instance;
	}
	LunarEngine::LResult GetRootSignature(const std::string& name_in, LunarObjectID& root_signature_id);
	LunarEngine::LResult GetResource(const LunarObjectID& root_signature_id, ID3D12RootSignature** root_signature_res);
	~PancyRootSignatureControl();
private:
	LunarEngine::LResult BuildRootSignature(const std::string& rootsig_config_file);
	void AddRootSignatureGlobelVariable();
};

//PSO object
class PancyPiplineStateObjectGraph
{
	PSOType pipline_type;
	//std::unordered_map<std::string,Json::Value> Cbuffer_map;
	std::unordered_map<std::string, ConstantBufferAlloctor*> Cbuffer_map;
	//todo：区分绑定资源的格式
	std::vector<PancyDescriptorPSODescription> globel_cbuffer;
	std::vector<PancyDescriptorPSODescription> private_cbuffer;
	std::vector<PancyDescriptorPSODescription> globel_shader_res;
	std::vector<PancyDescriptorPSODescription> private_shader_res;
	std::vector<PancyDescriptorPSODescription> bindless_shader_res;

	LunarObjectID root_signature_ID;
	LunarEngine::LString pso_name;
	ComPtr<ID3D12PipelineState> pso_data;
public:
	PancyPiplineStateObjectGraph(const std::string& pso_name_in);
	~PancyPiplineStateObjectGraph();
	LunarEngine::LResult Create();
	inline void GetResource(ID3D12PipelineState** res_data)
	{
		*res_data = pso_data.Get();
	}
	inline LunarObjectID GetRootSignature()
	{
		return root_signature_ID;
	}
	//LunarEngine::EngineFailReason GetDescriptorHeapUse(std::string  &descriptor_heap_name);
	//LunarEngine::EngineFailReason GetDescriptorDistribute(std::vector<LunarObjectID> &descriptor_distribute);
	//LunarEngine::EngineFailReason CheckCbuffer(const std::string &cbuffer_name);
	//LunarEngine::EngineFailReason GetCbuffer(const std::string &cbuffer_name, const Json::Value *& CbufferData);
	LunarEngine::LResult BuildCbufferByName(const std::string& cbuffer_name, PancyConstantBuffer& cbuffer_data_out);
	LunarEngine::LResult ReleaseCbufferByID(
		const std::string& cbuffer_name,
		const LunarObjectID& buffer_resource_id,
		const LunarObjectID& buffer_offset_id
	);
	const std::vector<PancyDescriptorPSODescription>& GetDescriptor(const PancyShaderDescriptorType& descriptor_type);
private:
	LunarEngine::LResult GetInputDesc(ComPtr<ID3D12ShaderReflection> t_ShaderReflection, std::vector<D3D12_INPUT_ELEMENT_DESC>& t_InputElementDescVec);
	LunarEngine::LResult BuildCbufferByShaderReflect(ComPtr<ID3D12ShaderReflection>& now_shader_reflect);
	LunarEngine::LResult ParseDiscriptorDistribution(const std::vector<DescriptorTypeDesc>& descriptor_desc_in);
};


//pso管理器
class PancyEffectGraphic
{
	//空闲的PSO ID号
	LunarObjectID PSO_id_self_add;
	std::queue<LunarObjectID> empty_PSO_id;
	//PSO的存储结构
	std::unordered_map<std::string, LunarObjectID> PSO_name;
	std::unordered_map<LunarObjectID, std::string> PSO_name_reflect;
	std::unordered_map<LunarObjectID, PancyPiplineStateObjectGraph*> PSO_array;
private:
	PancyEffectGraphic();
public:
	static PancyEffectGraphic* GetInstance()
	{
		static PancyEffectGraphic* this_instance;
		if (this_instance == NULL)
		{
			this_instance = new PancyEffectGraphic();
		}
		return this_instance;
	}
	LunarEngine::LResult GetPSO(const std::string& name_in, LunarObjectID& PSO_id);
	LunarEngine::LResult GetPSOResource(const LunarObjectID& PSO_id, ID3D12PipelineState** PSO_res);
	LunarEngine::LResult GetRootSignatureResource(const LunarObjectID& PSO_id, ID3D12RootSignature** RootSig_res);
	LunarEngine::LResult GetPSOName(const LunarObjectID& PSO_id, std::string& pso_name_out);
	//LunarEngine::EngineFailReason GetPSODescriptorName(const LunarObjectID &PSO_id, std::string &descriptor_heap_name);
	//LunarEngine::EngineFailReason GetDescriptorDistribute(const LunarObjectID &PSO_id, std::vector<LunarObjectID> &descriptor_distribute);
	//LunarEngine::EngineFailReason CheckCbuffer(const LunarObjectID &PSO_id, const std::string &name_in);
	//LunarEngine::EngineFailReason GetCbuffer(const LunarObjectID &PSO_id, const std::string &cbuffer_name, const Json::Value *& CbufferData);
	LunarEngine::LResult BuildCbufferByName(const LunarObjectID& PSO_id, const std::string& cbuffer_name, PancyConstantBuffer& cbuffer_data_out);
	LunarEngine::LResult ReleaseCbufferByID(
		const LunarObjectID& PSO_id,
		const std::string& cbuffer_name,
		const LunarObjectID& buffer_resource_id,
		const LunarObjectID& buffer_offset_id
	);


	LunarEngine::LResult GetDescriptorDesc(
		const LunarObjectID& PSO_id,
		const PancyShaderDescriptorType& descriptor_type,
		const std::vector<PancyDescriptorPSODescription>*& descriptor_param_data
	);
	~PancyEffectGraphic();
private:
	LunarEngine::LResult BuildPso(const std::string& pso_config_file);
	void AddPSOGlobelVariable();
	void AddJsonReflect();
};
