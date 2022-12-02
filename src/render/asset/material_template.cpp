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
	cls->Property<&Self::mTemplateParams>("params");
	cls->Property<&Self::mShader>("shader");
}

MaterialTemplateAsset::MaterialTemplateAsset() :
	mTemplateParams(this),
	mShader(this)
{
}

MaterialInstance* MaterialTemplateAsset::CreateInstance()
{
	MaterialInstance* mat = NewObject<MaterialInstance>();
	for (TSubPtr<MaterialParam>& p : mTemplateParams)
	{
		auto& params = mat->GetAllParams();
		params.PushBack(p.Get());
	}
	return mat;
}

void MaterialTemplateAsset::OnLoad()
{

}

}
