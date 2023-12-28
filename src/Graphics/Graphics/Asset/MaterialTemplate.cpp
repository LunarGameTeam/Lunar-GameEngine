#include "Graphics/Asset/MaterialTemplate.h"
#include "Core/Asset/Asset.h"
#include "Core/Asset/AssetModule.h"

#include "Core/Memory/PtrBinding.h"

#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/RenderModule.h"

namespace luna
{

REGISTER_ENUM_TYPE(graphics::RHIPrimitiveTopologyType, int)
REGISTER_ENUM_TYPE(graphics::RHIRasterizerCullMode, int)

namespace binding
{

template<>
struct binding_converter<graphics::RHIPrimitiveTopologyType> : enum_converter< graphics::RHIPrimitiveTopologyType> {};

template<>
struct binding_converter<graphics::RHIRasterizerCullMode> : enum_converter< graphics::RHIRasterizerCullMode> {};

}


}
namespace luna::graphics
{
RegisterTypeEmbedd_Imp(MaterialBaseTemplateAsset)
{
	cls->Ctor<MaterialBaseTemplateAsset>();

	cls->BindingProperty<&Self::mTemplateMacros>("macros")
		.Serialize();

	cls->BindingProperty<&Self::mShaderAsset>("shader")
		.Serialize();

	cls->Binding<MaterialBaseTemplateAsset>();
	BindingModule::Get("luna")->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialComputeAsset)
{
	cls->Ctor<MaterialComputeAsset>();
	cls->Binding<MaterialComputeAsset>();
	BindingModule::Get("luna")->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialGraphAsset)
{
	cls->Ctor<MaterialGraphAsset>();

	cls->BindingProperty<&Self::mDepthTestEnable>("depth_test_enable")
		.Serialize();

	cls->BindingProperty<&MaterialGraphAsset::mPrimitiveType>("primitive_type")
		.Getter<&MaterialGraphAsset::GetPrimitiveType>()
		.Setter< &MaterialGraphAsset::SetPrimitiveType>()
		.Serialize();

	cls->BindingProperty<&MaterialGraphAsset::mCullMode>("cull_mode")
		.Getter<&MaterialGraphAsset::GetCullMode>()
		.Setter< &MaterialGraphAsset::SetCullMode>()
		.Serialize();


	cls->BindingProperty<&Self::mDepthWriteEnable>("depth_write_enable")
		.Serialize();

	cls->Binding<MaterialGraphAsset>();
	BindingModule::Get("luna")->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialTemplateAsset)
{
	cls->Ctor<MaterialTemplateAsset>();

	cls->BindingProperty<&Self::mTemplateParams>("params")
		.Serialize();

	cls->Binding<MaterialTemplateAsset>();

	BindingModule::Get("luna")->AddType(cls);
}

//基础材质
MaterialBaseTemplateAsset::MaterialBaseTemplateAsset() :
	mTemplateMacros(this)
{

}

void MaterialBaseTemplateAsset::OnLoad()
{
	for (auto& it : mTemplateMacros)
	{
		it->SetParent(this);
	}
	LArray<ShaderMacro*> shaderMacros;
	for (int32_t macroId = 0; macroId < mTemplateMacros.Size(); ++macroId)
	{
		shaderMacros.push_back(mTemplateMacros[macroId]);
	}
	CompileShaderAsset(mShaderAsset.get(), shaderMacros);
}

void MaterialBaseTemplateAsset::CompileShaderByType(
	ShaderAsset* curShader,
	const LArray<ShaderMacro*>& shaderMacros,
	RHIShaderType shaderType
)
{
	SharedPtr<LShaderInstance> mShaderInstance = curShader->GenerateShaderInstance(shaderType, shaderMacros);
	mShaderInstances.insert({ RHIShaderType::Compute ,mShaderInstance});
}

void MaterialBaseTemplateAsset::CompileShaderAndLayoutByType(
	ShaderAsset* curShader,
	const LArray<ShaderMacro*>& shaderMacros,
	const LArray<RHIShaderType>& shaderType
)
{
	for (const RHIShaderType& curType : shaderType)
	{
		CompileShaderByType(curShader, shaderMacros, curType);
	}
	LArray<RHIShaderBlob*> shaderPack;
	for (auto& itor : mShaderInstances)
	{
		shaderPack.push_back(itor.second->GetRhiShader().get());
	}
	mLayout = GenerateAndCompileShaderLayout(sRenderModule->GetRHIDevice(), shaderPack);
}

LShaderInstance* MaterialBaseTemplateAsset::GetShaderByType(RHIShaderType type)
{
	auto itor = mShaderInstances.find(type);
	if (itor == mShaderInstances.end())
	{
		return nullptr;
	}
	return itor->second.get();
}

//计算材质
MaterialComputeAsset::MaterialComputeAsset()
{

}

MaterialComputeAsset::~MaterialComputeAsset()
{

}

void MaterialComputeAsset::CompileShaderAsset(ShaderAsset* curShader, const LArray<ShaderMacro*>& shaderMacros)
{
	LArray<RHIShaderType> shaderType;
	shaderType.push_back(RHIShaderType::Compute);
	CompileShaderAndLayoutByType(curShader, shaderMacros, shaderType);
}

LShaderInstance* MaterialComputeAsset::GetShaderComputeInstance()
{ 
	return GetShaderByType(RHIShaderType::Compute);
}

MaterialInstanceBase* MaterialComputeAsset::CreateInstance()
{
	MaterialInstanceComputeBase* mat = NewObject<MaterialInstanceComputeBase>();
	mat->SetAsset(this);
	return mat;
}

//渲染材质
MaterialGraphAsset::MaterialGraphAsset() : mDefaultInstance(this)
{
}

MaterialGraphAsset::~MaterialGraphAsset()
{
}

MaterialInstanceBase* MaterialGraphAsset::CreateInstance()
{
	MaterialInstanceGraphBase* mat = NewObject<MaterialInstanceGraphBase>();
	mat->SetAsset(this);
	return mat;
}

LShaderInstance* MaterialGraphAsset::GetShaderVertexInstance()
{ 
	return GetShaderByType(RHIShaderType::Vertex);
}

LShaderInstance* MaterialGraphAsset::GetShaderPixelInstance() 
{ 
	return GetShaderByType(RHIShaderType::Pixel);
}

RHIRasterizerCullMode MaterialGraphAsset::GetCullMode()
{
	return mCullMode;
}

void MaterialGraphAsset::SetCullMode(RHIRasterizerCullMode val)
{
	mCullMode = val;
}

bool MaterialGraphAsset::IsDepthWriteEnable()
{
	return mDepthWriteEnable;
}

void MaterialGraphAsset::SetDepthWriteEnable(bool val)
{
	mDepthWriteEnable = val;
}

bool MaterialGraphAsset::IsDepthTestEnable()
{
	return mDepthTestEnable;
}

void MaterialGraphAsset::SetDepthTestEnable(bool val)
{
	mDepthTestEnable = val;
}

void MaterialGraphAsset::CompileShaderAsset(ShaderAsset* curShader, const LArray<ShaderMacro*>& shaderMacros)
{
	LArray<RHIShaderType> shaderType;
	LArray<SharedPtr<LShaderInstance>> shaderInstanceOut;
	shaderType.push_back(RHIShaderType::Vertex);
	shaderType.push_back(RHIShaderType::Pixel);
	CompileShaderAndLayoutByType(curShader, shaderMacros, shaderType);
}

//标准材质
TPPtrArray<MaterialParam>& MaterialTemplateAsset::GetTemplateParams()
{ 
	return mTemplateParams;
}

MaterialInstanceBase* MaterialTemplateAsset::CreateInstance()
{
	MaterialInstance* mat = NewObject<MaterialInstance>();
	mat->SetAsset(this);
	mat->Ready();
	return mat;
}

}
