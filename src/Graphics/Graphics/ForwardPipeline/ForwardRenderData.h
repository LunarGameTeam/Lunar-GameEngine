#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/RenderData.h"


namespace  luna::graphics
{

struct ViewTargetData : public RenderData
{
	LSharedPtr<FGTexture> mRenderTarget = nullptr;
	LSharedPtr<FGTexture> mDepthStencil = nullptr;
};

PARAM_ID(_LUTTex);
PARAM_ID(_EnvTex);
PARAM_ID(_IrradianceTex);
struct SceneRenderData : public RenderData
{
	LSharedPtr<TextureCube> mEnvTex;
	LSharedPtr<TextureCube> mIrradianceTex;
	LSharedPtr<Texture2D>   mLUTTex;
public:
	void SetMaterialParameter(MaterialInstanceBase* matInstance);
};
void SceneRenderData::SetMaterialParameter(MaterialInstanceBase* matInstance)
{
	matInstance->SetShaderInput(ParamID__EnvTex, mEnvTex->GetView());
	matInstance->SetShaderInput(ParamID__IrradianceTex, mIrradianceTex->GetView());
	matInstance->SetShaderInput(ParamID__LUTTex, mLUTTex->GetView());
}


}