#pragma once
#include "Graphics/Renderer/SceneRenderer.h"
namespace luna::graphics
{
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
			eachGenerator->AddPassNode(frameGraph, curView);
		}
		for (FrameGraphPassGeneratorPerView* eachGenerator : valueOut)
		{
			eachGenerator->ClearRoQueue();
		}
	}

	void SceneRenderer::PrepareSceneRender(RenderScene* renderScene)
	{
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
		//		PBRPreparePass(FG, this, scene);
		//		DirectionalLightShadowPass(FG, this, scene);
		//		PointShadowPass(FG, this, scene);
		//		OpaquePass(FG, this, scene);
		//		PostProcessPass(FG, this, scene);
		//		OverlayPass(FG, this, scene);
	}

	void SceneRenderer::GeneratePassByView(RenderScene* renderScene,RenderView* curView)
	{
		curView->Culling(renderScene);
		mRenderPipeline.GeneratePerViwewPass(&mFrameGraphBuilder, curView);
	}

	void SceneRenderer::Render(RenderScene* renderScene)
	{
		renderScene->GetAllView(mAllViews);
		mFrameGraphBuilder.Clear();
		PrepareSceneRender(renderScene);
		for (RenderView* curView : mAllViews)
		{
			GeneratePassByView(renderScene,curView);
		}
		mFrameGraphBuilder.Flush();
	}
}