#include "Graphics/Asset/MaterialTemplate.h"
#include "Core/Asset/Asset.h"
#include "Core/Asset/AssetModule.h"

#include "Core/Memory/PtrBinding.h"

#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/RenderModule.h"

namespace luna
{

REGISTER_ENUM_TYPE(render::RHIPrimitiveTopologyType, int)
REGISTER_ENUM_TYPE(render::RHIRasterizerCullMode, int)

namespace binding
{

template<>
struct binding_converter<render::RHIPrimitiveTopologyType> : enum_converter< render::RHIPrimitiveTopologyType> {};

template<>
struct binding_converter<render::RHIRasterizerCullMode> : enum_converter< render::RHIRasterizerCullMode> {};

}


}
namespace luna::render
{

RegisterTypeEmbedd_Imp(MaterialTemplateAsset)
{
	cls->Ctor<MaterialTemplateAsset>();

	cls->BindingProperty<&Self::mTemplateParams>("params")
		.Serialize();

	cls->BindingProperty<&Self::mTemplateMacros>("macros")
		.Serialize();

	cls->BindingProperty<&Self::mShader>("shader")
		.Serialize();

	cls->BindingProperty<&Self::mDepthTestEnable>("depth_test_enable")
		.Serialize(); 

	cls->BindingProperty<&MaterialTemplateAsset::mPrimitiveType>("primitive_type")
		.Getter<&MaterialTemplateAsset::GetPrimitiveType>()
		.Setter< &MaterialTemplateAsset::SetPrimitiveType>()
		.Serialize();

	cls->BindingProperty<&MaterialTemplateAsset::mCullMode>("cull_mode")
		.Getter<&MaterialTemplateAsset::GetCullMode>()
		.Setter< &MaterialTemplateAsset::SetCullMode>()
		.Serialize();


	cls->BindingProperty<&Self::mDepthWriteEnable>("depth_write_enable")
		.Serialize();

	cls->Binding<MaterialTemplateAsset>();
	BindingModule::Get("luna")->AddType(cls);
}

MaterialTemplateAsset::MaterialTemplateAsset() :
	mTemplateParams(this),
	mDefaultInstance(this),
	mTemplateMacros(this)
{
}

MaterialTemplateAsset::~MaterialTemplateAsset()
{
}

MaterialInstance* MaterialTemplateAsset::CreateInstance()
{
	MaterialInstance* mat = NewObject<MaterialInstance>();
	mat->mMaterialTemplate = ToSharedPtr(this);
	mat->Ready();
	return mat;
}

void MaterialTemplateAsset::OnLoad()
{
	for (auto& it : mTemplateParams)
	{
		it->SetParent(this);
	}
	for (auto& it : mTemplateMacros)
	{
		it->SetParent(this);
	}
	LArray<ShaderMacro*> shaderMacros;
	for (int32_t macroId = 0; macroId < mTemplateMacros.Size(); ++macroId)
	{
		shaderMacros.push_back(mTemplateMacros[macroId]);
	}
	mVs = mShader->GenerateShaderInstance(RHIShaderType::Vertex, shaderMacros);
	mPs = mShader->GenerateShaderInstance(RHIShaderType::Pixel, shaderMacros);

	std::vector<RHIBindPoint> bindingKeys;
	std::map<std::tuple<uint32_t, uint32_t>, RHIBindPoint> result;
	for (auto& it : mVs->GetRhiShader()->mBindPoints)
	{
		auto& bindKey = it.second;
		result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
	}
	for (auto& it : mPs->GetRhiShader()->mBindPoints)
	{
		auto& bindKey = it.second;
		result[std::make_tuple(bindKey.mSpace, bindKey.mSlot)] = bindKey;
	}
	for (auto it : result)
	{
		bindingKeys.push_back(it.second);
	}
	mLayout = sRenderModule->GetRHIDevice()->CreateBindingSetLayout(bindingKeys);
}

RHIRasterizerCullMode MaterialTemplateAsset::GetCullMode()
{
	return mCullMode;
}

void MaterialTemplateAsset::SetCullMode(RHIRasterizerCullMode val)
{
	mCullMode = val;
}

bool MaterialTemplateAsset::IsDepthWriteEnable()
{
	return mDepthWriteEnable;
}

void MaterialTemplateAsset::SetDepthWriteEnable(bool val)
{
	mDepthWriteEnable = val;
}

bool MaterialTemplateAsset::IsDepthTestEnable()
{
	return mDepthTestEnable;
}

void MaterialTemplateAsset::SetDepthTestEnable(bool val)
{
	mDepthTestEnable = val;
}

}
