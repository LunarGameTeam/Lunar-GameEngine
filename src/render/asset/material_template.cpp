#include "material_template.h"
#include "core/asset/asset.h"
#include "core/asset/asset_module.h"

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

	cls->Binding<MaterialTemplateAsset>();
	BindingModule::Get("luna")->AddType(cls);
}

MaterialTemplateAsset::MaterialTemplateAsset() :
	mTemplateParams(this),
	mShader(this)
{
}

MaterialInstance* MaterialTemplateAsset::CreateInstance()
{
	MaterialInstance* mat = NewObject<MaterialInstance>();
	for (auto& p : mTemplateParams)
	{
		auto& params = mat->GetAllParams();
		params.PushBack(p.Get());
	}
	return mat;
}

void MaterialTemplateAsset::OnLoad()
{
	for (auto& it : mTemplateParams)
	{
		it->SetParent(this);
	}
}

}
