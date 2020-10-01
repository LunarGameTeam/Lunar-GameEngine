#pragma once
#include"PancyRenderParam.h"
namespace LunarEngine
{
	enum MaterialShaderResourceType
	{
		MaterialShaderResourceBufferBind = 0,
		MaterialShaderResourceTextureBind,
		MaterialShaderResourceBufferBindLess,
		MaterialShaderResourceTextureBindLess
	};
	//buffer资源的描述符格式
	struct PancyBufferDescriptorDesc
	{
		//基础属性
		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
		UINT Shader4ComponentMapping = 0;
		//buffer属性
		UINT64 FirstElement = 0;
		UINT NumElements = 0;
		UINT StructureByteStride = 0;
		D3D12_BUFFER_SRV_FLAGS Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	};
	INIT_REFLECTION_CLASS(PancyBufferDescriptorDesc,
		reflect_data.Format,
		reflect_data.Shader4ComponentMapping,
		reflect_data.FirstElement,
		reflect_data.NumElements,
		reflect_data.StructureByteStride,
		reflect_data.Flags
	);
	//buffer资源的shader绑定格式
	struct PancyMaterialShaderResourceDataBuffer
	{
		MaterialShaderResourceType shader_resource_type = MaterialShaderResourceBufferBind;
		std::string shader_resource_slot_name;
		std::vector<std::string> shader_resource_path;
		std::vector<PancyBufferDescriptorDesc> buffer_descriptor_desc;//buffer的每个buffer的格式(非bindless则只有一个)
	};
	INIT_REFLECTION_CLASS(PancyMaterialShaderResourceDataBuffer,
		reflect_data.shader_resource_type,
		reflect_data.shader_resource_slot_name,
		reflect_data.shader_resource_path,
		reflect_data.buffer_descriptor_desc
	);
	//texture资源的描述符格式
	struct PancyTextureDescriptorDesc
	{
		//基础属性
		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
		D3D12_SRV_DIMENSION ViewDimension = D3D12_SRV_DIMENSION_UNKNOWN;
		UINT Shader4ComponentMapping = 0;
		//纹理属性
		UINT MostDetailedMip = 0;
		UINT MipLevels = 0;
		UINT PlaneSlice = 0;
		UINT First2DArrayFace = 0;
		UINT NumCubes = 0;
		UINT FirstArraySlice = 0;
		UINT ArraySize = 0;
		UINT UnusedField_NothingToDefine = 0;
		FLOAT ResourceMinLODClamp = 0;
	};
	INIT_REFLECTION_CLASS(PancyTextureDescriptorDesc,
		reflect_data.Format,
		reflect_data.ViewDimension,
		reflect_data.Shader4ComponentMapping,
		reflect_data.MostDetailedMip,
		reflect_data.MipLevels,
		reflect_data.PlaneSlice,
		reflect_data.First2DArrayFace,
		reflect_data.FirstArraySlice,
		reflect_data.NumCubes,
		reflect_data.ArraySize,
		reflect_data.UnusedField_NothingToDefine,
		reflect_data.ResourceMinLODClamp
	);
	//texture资源的shader绑定格式
	struct PancyMaterialShaderResourceDataTexture
	{
		MaterialShaderResourceType shader_resource_type = MaterialShaderResourceBufferBind;
		std::string shader_resource_slot_name;
		std::vector<std::string> shader_resource_path;
		std::vector<PancyTextureDescriptorDesc> texture_descriptor_desc;//纹理的每个buffer的格式(非bindless则只有一个)
	};
	INIT_REFLECTION_CLASS(PancyMaterialShaderResourceDataTexture,
		reflect_data.shader_resource_type,
		reflect_data.shader_resource_slot_name,
		reflect_data.shader_resource_path,
		reflect_data.texture_descriptor_desc
	);
	//普通材质的输入格式
	struct PancyCommonMaterialDesc
	{
		std::string pipeline_state_name;
		//所有用到的缓冲区
		std::vector<PancyMaterialShaderResourceDataBuffer> marterial_buffer_slot_data;
		//所有用到的纹理
		std::vector<PancyMaterialShaderResourceDataTexture> marterial_texture_slot_data;
	};
	INIT_REFLECTION_CLASS(PancyCommonMaterialDesc,
		reflect_data.pipeline_state_name,
		reflect_data.marterial_buffer_slot_data,
		reflect_data.marterial_texture_slot_data
	);
	class PancyMaterialBasic : public PancyCommonVirtualResource<PancyCommonMaterialDesc>
	{
		std::vector<VirtualResourcePointer> ShaderResourceData;
		//渲染所需的描述符数据
		std::unordered_map<std::string, BindDescriptorPointer> bind_shader_resource;         //私有描述符(材质专用的绑定资源，需要传入或创建)
		std::unordered_map<std::string, BindlessDescriptorPointer> bindless_shader_resource; //解绑定描述符(材质专用的解绑定资源，需要传入或创建)
	public:
		PancyMaterialBasic(const bool& if_could_reload_in);
		//根据资源格式创建资源数据
		LunarEngine::LResult LoadResoureDataByDesc(const PancyCommonMaterialDesc& ResourceDescStruct) override;
		//创建一个渲染param
		LunarEngine::LResult BuildRenderParam(PancyRenderParamID& render_param_id);
		bool CheckIfResourceLoadFinish() override;;
	private:
		LunarEngine::LResult BuildBufferDescriptorByDesc(
			const std::string& resource_file_name,
			const PancyBufferDescriptorDesc& descriptor_desc,
			std::vector<VirtualResourcePointer>& resource_pointer_array,
			std::vector<BasicDescriptorDesc>& resource_descriptor_descr_array
		);
		LunarEngine::LResult BuildTextureDescriptorByDesc(
			const std::string& resource_file_name,
			const PancyTextureDescriptorDesc& descriptor_desc,
			std::vector<VirtualResourcePointer>& resource_pointer_array,
			std::vector<BasicDescriptorDesc>& resource_descriptor_descr_array
		);
	};
	LunarEngine::LResult LoadMaterialFromFile(
		const std::string& name_resource_in,
		VirtualResourcePointer& id_need
	);
	void InitMaterialJsonReflect();
}
