#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/RenderData.h"


namespace  luna::render
{

struct ViewShadowData : public RenderData
{
	FGTexture* mPointShadowmap = nullptr;
	FGTexture* mDirectionLightShadowmap = nullptr;

};

struct SceneRenderData : public RenderData
{
	FGTexture* mSceneColor = nullptr;
	FGTexture* mSceneDepth = nullptr;
	FGTexture* mLUTTex = nullptr;
	FGTexture* mEnvTex = nullptr;
	FGTexture* mIrradianceTex = nullptr;

};

}