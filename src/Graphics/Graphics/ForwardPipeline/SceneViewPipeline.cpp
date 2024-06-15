#pragma once
#include "Graphics/Renderer/SceneRenderer.h"

#include "Core/Asset/AssetModule.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Graphics/Renderer/FrameGraphPassGenerator.h"


namespace luna::graphics
{


LSharedPtr<RenderObjectDrawBatch> RunFilterROJob(RenderView* curView)
{
	graphics::RenderObjectDrawData* roParamData = curView->mOwnerScene->GetData<graphics::RenderObjectDrawData>();
	LArray<RenderObject*> mRoQueue;
	LArray<RenderObject*>& allObjects = curView->GetViewVisibleROs();
	for (RenderObject* eachRo : allObjects)
	{
		mRoQueue.push_back(eachRo);
	}
	auto batch = MakeShared<RenderObjectDrawBatch>(roParamData->mDrawBatch.size());
	roParamData->mDrawBatch.push_back(batch);
	batch->SetRenderObjects(curView, mRoQueue);
	return batch;
}

void OpaquePass(FrameGraphBuilder* builder, RenderScene* renderScene, RenderView* view)
{
	LSharedPtr<RenderObjectDrawBatch> batch(RunFilterROJob(view));

	FGGraphDrawNode* node = builder->AddGraphDrawPass("Opaque");

	SceneRenderData* sceneRenderData = renderScene->RequireData<SceneRenderData>();
	RenderObjectDrawData* roParamData = view->mOwnerScene->GetData<graphics::RenderObjectDrawData>();
	RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();

	for (auto& eachCommand : batch->mAllCommandsPool)
	{
		if (eachCommand.second.mDrawCount == 0)
		{
			continue;
		}
		sceneRenderData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
		viewParamData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
		roParamData->SetMaterialParameter(eachCommand.second.mDrawParameter.mMtl);
	}
	//绑定rt输出
	SceneViewTargetData* viewRtData = view->RequireData<SceneViewTargetData>();
	viewRtData->GenerateOpaqueResultRenderTarget(builder, node);

	node->ExcuteFunc([=](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
		{
			batch->DrawCommandBatch(cmdlist);
		});

}
void ShadowCastPass(FrameGraphBuilder* builder, RenderScene* scene, RenderView* view)
{
	static SharedPtr<MaterialGraphAsset> mShadowMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/Depth.mat");
	static MaterialInstanceGraphBase* mShadowDefaultMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mShadowMtlAsset->GetDefaultInstance());
	
	if (view->mViewType != RenderViewType::ShadowMapView)
	{
		return;
	}

	RunFilterROJob();

	RenderScene* renderScene = view->mOwnerScene;

	FGGraphDrawNode* node = builder->AddGraphDrawPass("Directional LightShadowmap");

	graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();

	graphics::RenderObjectDrawData*  roParamData = renderScene->GetData<graphics::RenderObjectDrawData>();

	viewParamData->SetMaterialParameter(mShadowDefaultMtlInstance);

	roParamData->SetMaterialParameter(mShadowDefaultMtlInstance);

	ShadowViewTargetData* viewRtData = view->RequireData<ShadowViewTargetData>();

	viewRtData->GenerateShadowRenderTarget(builder, node);

	node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
		{
			DrawCommandBatch(cmdlist);
		});
}

PARAM_ID(_SkyTex);
void SkyboxPass(FrameGraphBuilder* builder, RenderScene* scene, RenderView* view)
{

	static SharedPtr<MeshAsset> mSkyBoxMeshAsset = sAssetModule->LoadAsset<MeshAsset>("/assets/built-in/Geometry/InsideSphere.lmesh");

	static RenderAssetDataMesh* mSkyboxRenderMesh = mSkyBoxMeshAsset->GetSubMeshAt(0)->GetRenderMeshData();

	static SharedPtr<MaterialGraphAsset> mSkyBoxMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/Skybox/Skybox.mat");

	static MaterialInstanceGraphBase* mSkyBoxDefaultMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mSkyBoxMtlAsset->GetDefaultInstance());

	if (view->mViewType != RenderViewType::SceneView)
	{
		return;
	}
	RenderScene* renderScene = view->mOwnerScene;
	FGGraphDrawNode* node = builder->AddGraphDrawPass("SkyBox");
	SceneRenderData* sceneRenderData = renderScene->RequireData<SceneRenderData>();
	if (sceneRenderData->mEnvTex == nullptr)
	{
		sceneRenderData->mEnvTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>("/assets/built-in/Textures/Cubemap.dds"));
		sceneRenderData->mIrradianceTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>("/assets/built-in/Textures/IrradianceMap.dds"));
		sceneRenderData->mLUTTex = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/Textures/brdf.dds"));
	}

	auto* viewParamData = view->GetData<graphics::RenderViewParameterData>();
	//sceneRenderData->SetMaterialParameter(mSkyBoxDefaultMtlInstance);
	mSkyBoxDefaultMtlInstance->SetShaderInput(ParamID__SkyTex, sceneRenderData->mEnvTex->GetView());
	PARAM_ID(_ClampSampler);
	mSkyBoxDefaultMtlInstance->SetShaderInput(ParamID__ClampSampler, sGlobelRhiResourceGenerator->GetClampSamper().mView);
	viewParamData->SetMaterialParameter(mSkyBoxDefaultMtlInstance);
	SceneViewTargetData* viewRtData = view->RequireData<SceneViewTargetData>();
	viewRtData->GenerateOpaqueResultRenderTarget(builder, node, true, true);

	node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
		{
			mSkyBoxDefaultMtlInstance->UpdateBindingSet();
			sRenderDrawContext->DrawMesh(cmdlist, mSkyboxRenderMesh, mSkyBoxDefaultMtlInstance);
		});
}

void PostProcess(FrameGraphBuilder* builder, RenderScene* scene, RenderView* view)
{
	static LSharedPtr<MaterialGraphAsset> mGammaCorrectionMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/GammaCorrection.mat");
	static MaterialInstanceGraphBase* mGammaCorrectionMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mGammaCorrectionMtlAsset->GetDefaultInstance());
	if (view->mViewType != RenderViewType::SceneView)
	{
		return;
	}
	FGGraphDrawNode* node = builder->AddGraphDrawPass("GammaCorrection");
	SceneViewTargetData* viewRtData = view->RequireData<SceneViewTargetData>();
	//暂时应该没有pass依赖后处理，先把后处理的结果直接写入屏幕
	viewRtData->GenerateScreenRenderTarget(builder, node);
	node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
		{
			sRenderDrawContext->DrawFullScreen(cmdlist, mGammaCorrectionMtlInstance);
		});
}

void OverlayPass(FrameGraphBuilder* builder, RenderScene* scene, RenderView* view)
{
	static RenderAssetDataMesh* mDebugMeshLineData = sRenderResourceContext->GetAssetManager()->GetDebugMeshLineMesh();

	static RenderAssetDataMesh* mDebugMeshData = sRenderResourceContext->GetAssetManager()->GetDebugMesh();

	static SharedPtr<MaterialGraphAsset> mOverlayMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/Line.mat");

	static MaterialInstanceGraphBase* mOverlayMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mOverlayMtlAsset->GetDefaultInstance());
	if (view->mViewType != RenderViewType::SceneView)
	{
		return;
	}
	FGGraphDrawNode* node = builder->AddGraphDrawPass("Overlay");
	graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();
	viewParamData->SetMaterialParameter(mOverlayMtlInstance);
	SceneViewTargetData* viewRtData = view->RequireData<SceneViewTargetData>();
	viewRtData->GenerateScreenRenderTarget(builder, node);
	node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
		{
			sRenderDrawContext->DrawMesh(cmdlist, mDebugMeshLineData, mOverlayMtlInstance);
			sRenderDrawContext->DrawMesh(cmdlist, mDebugMeshData, mOverlayMtlInstance);
		});
}


void SceneViewPipeline(FrameGraphBuilder* frameGraph, RenderScene* scene, RenderView* curView)
{
	auto roParamData = curView->mOwnerScene->GetData<graphics::RenderObjectDrawData>();
	roParamData->mDrawBatch.clear();

	RunFilterROJob(curView);
	
	SkyboxPass(frameGraph, scene, curView);
	OpaquePass(frameGraph, scene, curView);
	PostProcess(frameGraph, scene, curView);
	OverlayPass(frameGraph, scene, curView);
}


}
