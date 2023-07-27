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

#include "Graphics/ForwardPipeline/ForwardRenderData.h"

#include "Graphics/Renderer/MaterialParam.h"

namespace luna::graphics
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

void OverlayPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	auto& node = builder->AddPass("Overlay");
	FGResourceView* shadowmapView = nullptr;

	RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
	RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;
	FGTexture* color = builder->BindExternalTexture(colorTexture, "ViewTargetColor");
	FGTexture* depth = builder->BindExternalTexture(depthTexture, "ViewTargetDepth");

	assert(color);
	assert(depth);

	auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D);
	auto depthView = node.AddDSV(depth);

	node.SetColorAttachment(colorView, LoadOp::kLoad);
	node.SetDepthStencilAttachment(depthView);

	static auto debugMatAsset = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/Line.mat");
	static MaterialInstance* debugMat = debugMatAsset->GetDefaultInstance();
	if (debugMat)
		debugMat->Ready();
	node.ExcuteFunc([view, renderScene, shadowmapView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device) 
	{
		if (!renderScene->mDrawGizmos)
			return;
		PARAM_ID(MaterialBuffer);
		PARAM_ID(_MainTex);
		PARAM_ID(_ShadowMap);
		debugMat->SetShaderInput(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer->mView);
		debugMat->SetShaderInput(ParamID_ViewBuffer, view->mViewBuffer->mView);
		RHIResource* instancingBuffer = renderScene->mROIDInstancingBuffer->mRes;
		if(renderScene->mDebugMeshLineData.GetVertexSize() != 0)
			device->DrawMesh(&renderScene->mDebugMeshLineData, debugMat, nullptr);
		if (renderScene->mDebugMeshData.GetVertexSize() != 0)
			device->DrawMesh(&renderScene->mDebugMeshData, debugMat, nullptr);
	});
}

}

