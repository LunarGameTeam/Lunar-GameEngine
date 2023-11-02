#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/RenderData.h"


namespace  luna::graphics
{

struct ViewShadowData : public RenderData
{
	size_t pointShadowWidth;
	size_t pointShadowHeight;
	LSharedPtr<FGTexture> mPointShadowmap = nullptr;

	size_t pointShadowWidth;
	size_t pointShadowHeight;
	LSharedPtr<FGTexture> mDirectionLightShadowmap = nullptr;

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