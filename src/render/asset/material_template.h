#pragma once
#include "core/foundation/container.h"
#include "core/asset/asset.h"
#include "core/asset/json_asset.h"

#include "render/pch.h"
#include "render/asset/shader_asset.h"
#include "render/asset/texture_asset.h"
#include "render/renderer/material.h"

#include <d3d11.h>


namespace luna::render
{
class RENDER_API MaterialTemplateAsset : public LJsonAsset
{
	RegisterTypeEmbedd(MaterialTemplateAsset, LJsonAsset)

public:
	MaterialTemplateAsset();
	ShaderAsset* GetShaderAsset() { return mShader.Get(); }
	TSubPtrArray<MaterialParam>& GetAllParams() { return mTemplateParams; }
	MaterialInstance* CreateInstance();

public:
	void OnLoad() override;

private:
	TSubPtrArray<MaterialParam> mTemplateParams;
	TSubPtr<ShaderAsset> mShader;
};
}
