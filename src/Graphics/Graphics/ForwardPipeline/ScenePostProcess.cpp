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

namespace luna::graphics
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

void PostProcessPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	auto& node = builder->AddPass("GammaCorrection");
	FGResourceView* sceneView = nullptr;

	SceneRenderData* data = renderScene->RequireData<SceneRenderData>();

	RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
	FGTexture* color = builder->BindExternalTexture(colorTexture, "ViewTargetColor");

	assert(color);
	assert(data->mSceneColor);

	sceneView = node.AddSRV(data->mSceneColor, RHIViewDimension::TextureView2D);
	auto colorView = node.AddRTV(color, RHIViewDimension::TextureView2D);

	node.SetColorAttachment(colorView, LoadOp::kLoad);
	
	static auto postprocessMat = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/GammaCorrection.mat");
	static MaterialInstance* debugMat = postprocessMat->GetDefaultInstance();
	if (debugMat)
		debugMat->Ready();
	node.ExcuteFunc([view, renderScene, sceneView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		PARAM_ID(_MainTex);
		debugMat->SetShaderInput(ParamID__MainTex, sceneView->mRHIView);		
		debugMat->SetShaderInput(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer->mView);
		device->DrawMesh(&sRenderModule->mFullscreenMesh , debugMat, nullptr);
	});
}

}

