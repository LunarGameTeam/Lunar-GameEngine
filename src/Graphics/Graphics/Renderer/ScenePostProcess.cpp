#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/RenderModule.h"


namespace luna::render
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

void PostProcessPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	auto& node = builder->AddPass("GammaCorrection");
	FGResourceView* sceneView = nullptr;

	node.SetupFunc(builder, [&](FrameGraphBuilder* builder, FGNode& node)
	{
		SceneRenderData* data = renderScene->RequireData<SceneRenderData>();		
		
		RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
		RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;
		FGTexture* color = builder->BindExternalTexture( colorTexture, "ViewTargetColor");
		FGTexture* depth = builder->BindExternalTexture(depthTexture, "ViewTargetDepth");

		assert(color);
		assert(depth);
		assert(data->mSceneColor);

		sceneView = node.AddSRV(data->mSceneColor, RHIViewDimension::TextureView2D);
		auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D);
		auto depthView = node.AddDSV(depth);
		
		node.SetColorAttachment(colorView, LoadOp::kLoad);
		node.SetDepthStencilAttachment(depthView);
	});

	node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node)
	{

	});
	static MaterialTemplateAsset* postprocessMat = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/GammaCorrection.mat");
	static MaterialInstance* debugMat = postprocessMat->GetDefaultInstance();
	if (debugMat)
		debugMat->Ready();
	node.ExcuteFunc([view, renderScene, sceneView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		PackedParams params;
		PARAM_ID(_MainTex);
		PARAM_ID(_ShadowMap);
		params.PushShaderParam(ParamID__MainTex, sceneView->mRHIView);
		
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
		device->DrawMesh(sRenderModule->mFullscreenMesh, debugMat, &params);
	});
}

}

