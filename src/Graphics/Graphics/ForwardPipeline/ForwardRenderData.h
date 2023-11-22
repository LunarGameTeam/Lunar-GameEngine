#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/RenderData.h"


namespace  luna::graphics
{

struct ViewShadowData : public RenderData
{
	LSharedPtr<FGTexture> mLightShadowmap = nullptr;
	LSharedPtr<FGTexture> mLightShadowDepth = nullptr;
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