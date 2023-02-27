#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"



namespace luna::render
{
class RENDER_API MaterialTemplateAsset : public JsonAsset
{
	RegisterTypeEmbedd(MaterialTemplateAsset, JsonAsset)

public:
	MaterialTemplateAsset();
	ShaderAsset* GetShaderAsset() { return mShader.get(); }
	TPPtrArray<MaterialParam>& GetAllParams() { return mTemplateParams; }
	MaterialInstance* CreateInstance();
	MaterialInstance* GetDefaultInstance() { 
		if (!mDefaultInstance)
			mDefaultInstance = CreateInstance();
		return mDefaultInstance.Get();
	}

	RHIRasterizerCullMode GetCullMode();
	void SetCullMode(RHIRasterizerCullMode val);

	bool IsDepthWriteEnable();
	void SetDepthWriteEnable(bool val);

	bool IsDepthTestEnable();
	void SetDepthTestEnable(bool val);

	RHIPrimitiveTopologyType GetPrimitiveType() { return mPrimitiveType; }
	void SetPrimitiveType(RHIPrimitiveTopologyType val) { mPrimitiveType = val; }

public:
	void OnLoad() override;

private:
	RHIRasterizerCullMode mCullMode = RHIRasterizerCullMode::BackFace;
	bool mDepthTestEnable = true;
	bool mDepthWriteEnable = true;
	RHIPrimitiveTopologyType mPrimitiveType = RHIPrimitiveTopologyType::Triangle;
	
	TPPtr< MaterialInstance> mDefaultInstance;
	TPPtrArray<MaterialParam> mTemplateParams;

	SharedPtr<ShaderAsset> mShader;
};
}
