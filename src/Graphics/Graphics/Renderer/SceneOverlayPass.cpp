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

void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	auto& node = builder->AddPass("Overlay");
	FGResourceView* shadowmapView = nullptr;

	node.SetupFunc(builder, [&](FrameGraphBuilder* builder, FGNode& node)
	{
		LString rtName = "MainColor";
		LString rtDepthName = "MainDepth";
		RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
		RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;
		builder->BindExternalTexture(rtName, colorTexture);
		builder->BindExternalTexture(rtDepthName, depthTexture);

		FGTexture* color = builder->GetTexture(rtName);
		FGTexture* depth = builder->GetTexture(rtDepthName);

		assert(color);
		assert(depth);

		auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D);
		auto depthView = node.AddDSV(depth);

		node.SetColorAttachment(colorView, LoadOp::kLoad);
		node.SetDepthStencilAttachment(depthView);
	});

	node.PreExecFunc([view, renderScene](FrameGraphBuilder* builder, FGNode& node)
	{

	});
	static MaterialTemplateAsset* debugMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Line.mat");
	static MaterialInstance* debugMat = debugMatAsset->GetDefaultInstance();
	if (debugMat)
		debugMat->Ready();
	node.ExcuteFunc([view, renderScene, shadowmapView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device) 
	{
		if (!renderScene->mDrawGizmos)
			return;
		ROArray& ROs = view->GetViewVisibleROs();
		PackedParams params;
		PARAM_ID(MaterialBuffer);
		PARAM_ID(_MainTex);
		PARAM_ID(_ShadowMap);
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer);
		params.PushShaderParam(ParamID_ViewBuffer, view->mViewBuffer);
		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
		if(renderScene->mDebugMeshLine->mVertexData.size())
			device->DrawMesh(renderScene->mDebugMeshLine, debugMat, &params);
		if (renderScene->mDebugMesh->mVertexData.size())
			device->DrawMesh(renderScene->mDebugMesh, debugMat, &params);
	});
}

}

