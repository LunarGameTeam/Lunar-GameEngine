#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/RenderData.h"
#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/MaterialInstance.h"


namespace  luna::graphics
{

struct ViewTargetData : public RenderData
{
	LSharedPtr<FGTexture> mOpaqueResultRenderTarget = nullptr;

	LSharedPtr<FGTexture> mOpaqueResultDepthStencil = nullptr;

	LSharedPtr<FGTexture> mPostProcessResultRenderTarget = nullptr;

	LSharedPtr<FGTexture> mPostProcessResultDepthStencil = nullptr;

	LSharedPtr<FGTexture> mScreenRenderTarget = nullptr;

	LSharedPtr<FGTexture> mScreenDepthStencil = nullptr;
public:
	void GenerateOpaqueResultRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node);

	void GeneratePostProcessResultRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node);

	void GenerateScreenRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node);
private:
	void GenerateViewTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node, LSharedPtr<FGTexture>& colorRt, LSharedPtr<FGTexture>& depthRt);
};

struct ShadowViewTargetData : public RenderData
{
	LSharedPtr<FGTexture> mShadowRenderTarget = nullptr;

	LSharedPtr<FGTexture> mShadowDepthStencil = nullptr;
public:
	void GenerateShadowRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node);
};

struct SceneRenderData : public RenderData
{
	LSharedPtr<TextureCube> mEnvTex;

	LSharedPtr<TextureCube> mIrradianceTex;

	LSharedPtr<Texture2D>   mLUTTex;
public:
	SceneRenderData();
	void SetMaterialParameter(MaterialInstanceBase* matInstance);
};


}