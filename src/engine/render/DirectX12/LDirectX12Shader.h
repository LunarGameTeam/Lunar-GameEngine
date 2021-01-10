#pragma once
#include "LDirectXCommon.h"
namespace luna
{
	struct LunarShaderMessage
	{
		LunarGraphicShaderType shader_type;   //shader类型
		LString shader_path;              //shader文件路径
		LString shader_entry_point_name;  //shader入口函数
	};
	INIT_REFLECTION_CLASS(LunarShaderMessage,
		reflect_data.shader_type,
		reflect_data.shader_path,
		reflect_data.shader_entry_point_name
	);
	class LDx12GraphicResourceShaderBlob :public LTemplateAsset<LunarShaderMessage>
	{
		Microsoft::WRL::ComPtr<ID3DBlob> shader_memory_pointer;
		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shader_reflection;
		static LUnorderedMap<LunarGraphicShaderType, LString> shader_type_string;
	public:
		LDefaultAssert(LDx12GraphicResourceShaderBlob, LunarShaderMessage)
			inline ID3DBlob* GetShader()
		{
			return shader_memory_pointer.Get();
		}
		inline ID3D12ShaderReflection* GetShaderReflection()
		{
			return shader_reflection.Get();
		}
	private:
		void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override;
		luna::LResult InitResorceByDesc(const LunarShaderMessage& resource_desc) override;
	};
}
