#pragma once
#include "Graphics/Renderer/SceneRenderer.h"
#include"Graphics/ForwardPipeline/SceneShadowPass.h"
#include"Graphics/ForwardPipeline/SceneOpaquePass.h"
#include"Graphics/ForwardPipeline/SceneOverlayPass.h"
#include"Graphics/ForwardPipeline/ScenePostProcess.h"
#include"Graphics/ForwardPipeline/SkyBoxPass.h"
namespace luna::graphics
{
	void InitBasePipeline(SceneRenderPipeline* curPipeline)
	{
		curPipeline->InitNewPerViewFrameGraphGeneratorInstance<SkyBoxPassGenerator>();
		//curPipeline->InitNewPerViewFrameGraphGeneratorInstance<DirectionalLightShadowPassGenerator>();
		curPipeline->InitNewPerViewFrameGraphGeneratorInstance<OpaquePassGenerator>();
		//curPipeline->InitNewPerViewFrameGraphGeneratorInstance<SceneOverLayPassGenerator>();
		//curPipeline->InitNewPerViewFrameGraphGeneratorInstance<PostProcessPassGenerator>(); 
		
	}

	void SceneRenderPipeline::GeneratePerViwewPass(FrameGraphBuilder* frameGraph, RenderView* curView)
	{
		LArray<FrameGraphPassGeneratorPerView*> valueOut;
		mAllPassGeneratorPerView.GetAllValueList(valueOut);
		for (FrameGraphPassGeneratorPerView* eachGenerator : valueOut)
		{
			eachGenerator->FilterRenderObject(curView);
		}
		for (FrameGraphPassGeneratorPerView* eachGenerator : valueOut)
		{
			if (eachGenerator->GetRenderQueueSize() == 0)
			{
				continue;
			}
			eachGenerator->AddPassNode(frameGraph, curView);
		}
		for (FrameGraphPassGeneratorPerView* eachGenerator : valueOut)
		{
			eachGenerator->ClearRoQueue();
		}
	}

	void SceneRenderer::PrepareSceneRender(RenderScene* renderScene)
	{
		renderScene->GetRenderDataUpdater()->ExcuteCommand();

		for (auto data : renderScene->mDatas)
		{
			data.second->PerSceneUpdate(renderScene);
		}

		for (RenderView* curView : mAllViews)
		{
			for (auto data : curView->mDatas)
			{
				data.second->PerViewUpdate(curView);
			}
		}
	}

	void SceneRenderer::GeneratePassByView(RenderScene* renderScene,RenderView* curView)
	{
		curView->Culling(renderScene);
		mRenderPipeline.GeneratePerViwewPass(&mFrameGraphBuilder, curView);
	}

	void SceneRenderer::OnFrameBegin()
	{
		mFrameGraphBuilder.CleanUpVirtualMemory();
	}

	void SceneRenderer::Render(RenderScene* renderScene, RHICmdList* cmdList)
	{
		renderScene->GetAllView(mAllViews);
		mFrameGraphBuilder.Clear();
		for (RenderView* curView : mAllViews)
		{
			if (curView->GetRenderTarget() == nullptr)
			{
				continue;
			}
			GeneratePassByView(renderScene,curView);
		}
		mFrameGraphBuilder.Flush(cmdList);
	}
}