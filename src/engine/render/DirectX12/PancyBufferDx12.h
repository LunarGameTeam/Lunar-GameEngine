#pragma once

#include "core/core_module.h"
#include "PancyResourceBasic.h"
#include "PancyThreadBasic.h"

namespace LunarEngine
{
	enum PancyBufferType
	{
		Buffer_ShaderResource_static = 0,
		Buffer_ShaderResource_dynamic,
		Buffer_Constant,
		Buffer_Vertex,
		Buffer_Index,
		Buffer_UnorderedAccess_static
	};
	struct PancyCommonBufferDesc
	{
		PancyBufferType buffer_type;
		D3D12_RESOURCE_DESC buffer_res_desc = {};
		std::string buffer_data_file;
	};
	INIT_REFLECTION_CLASS(PancyCommonBufferDesc,
		reflect_data.buffer_type,
		reflect_data.buffer_res_desc.Dimension,
		reflect_data.buffer_res_desc.Alignment,
		reflect_data.buffer_res_desc.Width,
		reflect_data.buffer_res_desc.Height,
		reflect_data.buffer_res_desc.DepthOrArraySize,
		reflect_data.buffer_res_desc.MipLevels,
		reflect_data.buffer_res_desc.Format,
		reflect_data.buffer_res_desc.SampleDesc.Count,
		reflect_data.buffer_res_desc.SampleDesc.Quality,
		reflect_data.buffer_res_desc.Layout,
		reflect_data.buffer_res_desc.Flags,
		reflect_data.buffer_data_file
	);
	//缓冲区资源
	class PancyBasicBuffer : public PancyCommonVirtualResource<PancyCommonBufferDesc>
	{
		LunarResourceSize subresources_size = 0;
		UINT8* map_pointer = NULL;
		ResourceBlockGpu* buffer_data = nullptr;     //buffer数据指针
	public:
		PancyBasicBuffer(const bool& if_could_reload);
		~PancyBasicBuffer();
		inline const LunarResourceSize GetBufferSize() const
		{
			return subresources_size;
		}
		inline UINT8* GetBufferCPUPointer() const
		{
			return map_pointer;
		}
		inline ResourceBlockGpu* GetGpuResourceData() const
		{
			return buffer_data;
		}
		//检测当前的资源是否已经被载入GPU
		bool CheckIfResourceLoadFinish() override;
		//将cpu数据拷贝到buffer中
		LunarEngine::LResult WriteDataToBuffer(void* cpu_data_pointer, const LunarResourceSize& data_size);
	private:
		LunarEngine::LResult LoadResoureDataByDesc(const PancyCommonBufferDesc& ResourceDescStruct) override;
		LunarEngine::LResult CopyCpuDataToBufferGpu(void* cpu_data_pointer, const LunarResourceSize& data_size);
	};
	ResourceBlockGpu* GetBufferResourceData(VirtualResourcePointer& virtual_pointer, LunarEngine::LResult& check_error);
	LunarEngine::LResult BuildBufferResource(
		const std::string& name_resource_in,
		PancyCommonBufferDesc& resource_data,
		VirtualResourcePointer& id_need,
		bool if_allow_repeat
	);
	LunarEngine::LResult BuildBufferResourceFromMemory(
		const std::string& name_resource_in,
		VirtualResourcePointer& id_need,
		void* data_pointer,
		const LunarResourceSize& resource_size,
		bool if_allow_repeat,
		D3D12_RESOURCE_FLAGS flag = D3D12_RESOURCE_FLAG_NONE
	);
	LunarEngine::LResult LoadBufferResourceFromFile(
		const std::string& name_resource_in,
		VirtualResourcePointer& id_need
	);
	void InitBufferJsonReflect();
}