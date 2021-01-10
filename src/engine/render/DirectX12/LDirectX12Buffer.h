#pragma once
#include"LDirectX12Resource.h"
//buffer数据
namespace luna
{
	enum LunaBufferType
	{
		Buffer_ShaderResource_static = 0,
		Buffer_ShaderResource_dynamic,
		Buffer_Constant,
		Buffer_Vertex,
		Buffer_Index,
		Buffer_UnorderedAccess_static
	};
	struct LunaCommonBufferDesc
	{
		LunaBufferType buffer_type;
		D3D12_RESOURCE_DESC buffer_res_desc = {};
		std::string buffer_data_file;
	};
	INIT_REFLECTION_CLASS(LunaCommonBufferDesc,
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
	class LDx12GraphicResourceBuffer :public LTemplateAsset<LunaCommonBufferDesc>
	{
		UINT8* map_pointer = nullptr;
		size_t subresources_size = 0;
		LDirectx12ResourceBlock* buffer_data = nullptr;
	public:
		LDefaultAssert(LDx12GraphicResourceBuffer, LunaCommonBufferDesc)
		inline ID3D12Resource* GetResource()
		{
			if (buffer_data!= nullptr)
			{
				return buffer_data->GetResource();
			}
			return nullptr;
		}
		UINT8* GetMapPointer()
		{
			return map_pointer;
		}
	private:
		void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override;
		luna::LResult InitResorceByDesc(const LunaCommonBufferDesc& resource_desc) override;
	};
	void InitBufferJsonReflect();
}