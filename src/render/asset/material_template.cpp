#include "material_template.h"
#include "core/asset/asset.h"
#include "core/asset/asset_module.h"

#include "core/memory/ptr_binding.h"

#include "render/asset/material_template.h"
#include "render/asset/texture_asset.h"


namespace luna::render
{

RegisterTypeEmbedd_Imp(MaterialTemplateAsset)
{
	cls->Ctor<MaterialTemplateAsset>();

	cls->BindingProperty<&Self::mTemplateParams>("params")
		.Serialize();

	cls->BindingProperty<&Self::mShader>("shader")
		.Serialize();

	cls->BindingProperty<&Self::mDepthTestEnable>("depth_test_enable")
		.Serialize();

	cls->BindingProperty<&Self::mDepthWriteEnable>("depth_write_enable")
		.Serialize();

	cls->Binding<MaterialTemplateAsset>();
	BindingModule::Get("luna")->AddType(cls);
}

MaterialTemplateAsset::MaterialTemplateAsset() :
	mTemplateParams(this),
	mShader(this),
	mDefaultInstance(this)
{
}

MaterialInstance* MaterialTemplateAsset::CreateInstance()
{
	MaterialInstance* mat = NewObject<MaterialInstance>();
	mat->mMaterialTemplate = this;
	mat->Ready();
	return mat;
}

void MaterialTemplateAsset::OnLoad()
{
	for (auto& it : mTemplateParams)
	{
		it->SetParent(this);
	}
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
