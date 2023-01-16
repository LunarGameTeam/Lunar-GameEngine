#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"

#include <d3d11.h>


namespace luna::render
{
class RENDER_API MaterialTemplateAsset : public JsonAsset
{
	RegisterTypeEmbedd(MaterialTemplateAsset, JsonAsset)

public:
	MaterialTemplateAsset();
	ShaderAsset* GetShaderAsset() { return mShader.Get(); }
	TPPtrArray<MaterialParam>& GetAllParams() { return mTemplateParams; }
	MaterialInstance* CreateInstance();
	MaterialInstance* GetDefaultInstance() { 
		if (!mDefaultInstance)
			mDefaultInstance = CreateInstance();
		return mDefaultInstance.Get();
	}

	bool IsDepthWriteEnable();
	void SetDepthWriteEnable(bool val);

	bool IsDepthTestEnable();
	void SetDepthTestEnable(bool val);

public:
	void OnLoad() override;

private:
	bool mDepthTestEnable = true;
	bool mDepthWriteEnable = true;
	
	TPPtr< MaterialInstance> mDefaultInstance;
	TPPtrArray<MaterialParam> mTemplateParams;
	TPPtr<ShaderAsset> mShader;
};
}
