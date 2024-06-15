#pragma once

#include "Core/Asset/AssetModule.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Graphics/Renderer/FrameGraphPassGenerator.h"
#include "Graphics/Renderer/SceneViewPipeline.h"


namespace luna::graphics
{



void ShadowCastPass(FrameGraphBuilder* builder, RenderScene* renderScene, RenderView* view)
{
	assert (view->mViewType == RenderViewType::ShadowMapView);

	auto batch = RunFilterROJob(view);
	static SharedPtr<MaterialGraphAsset> mShadowMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/Depth.mat");
	static MaterialInstanceGraphBase* mShadowDefaultMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mShadowMtlAsset->GetDefaultInstance());
	
	FGGraphDrawNode* node = builder->AddGraphDrawPass("Directional LightShadowmap");

	graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();

	graphics::RenderObjectDrawData*  roParamData = renderScene->GetData<graphics::RenderObjectDrawData>();

	viewParamData->SetMaterialParameter(mShadowDefaultMtlInstance);

	roParamData->SetMaterialParameter(mShadowDefaultMtlInstance);

	ShadowViewTargetData* viewRtData = view->RequireData<ShadowViewTargetData>();

	viewRtData->GenerateShadowRenderTarget(builder, node);

	node->ExcuteFunc([=](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
		{
			batch->DrawCommandBatch(cmdlist);
		});
}

void ShadowCastViewPipeline(FrameGraphBuilder* frameGraph, RenderScene* scene, RenderView* curView)
{
	auto roParamData = curView->mOwnerScene->GetData<graphics::RenderObjectDrawData>();
	roParamData->mDrawBatch.clear();
	ShadowCastPass(frameGraph, scene, curView);
}


}
