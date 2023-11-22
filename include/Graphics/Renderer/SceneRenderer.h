#pragma once
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"
#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/Renderer/RenderView.h"
namespace luna::graphics
{
	class RENDER_API SceneRenderPipeline
	{
		LArray<FrameGraphPassGenerator*> mAllPassGenerator;
	public:
		void GeneratePass(FrameGraphBuilder* frameGraph, RenderScene* curScene, RenderView* curView);
		void InitNewFrameGraphGeneratorInstance(FrameGraphPassGenerator* newGenerator);
	};

	void SceneRenderPipeline::GeneratePass(FrameGraphBuilder* frameGraph, RenderScene* curScene, RenderView* curView)
	{
		LArray<RenderObject*>& allObjects = curView->GetViewVisibleROs();
		for (RenderObject* eachRo : allObjects)
		{
			for (FrameGraphPassGenerator* eachGenerator : mAllPassGenerator)
			{
				eachGenerator->FilterRenderObject(eachRo);
			}
		}
		for (FrameGraphPassGenerator* eachGenerator : mAllPassGenerator)
		{
			eachGenerator->AddPassNode(frameGraph, curView, curScene);
		}
		for (FrameGraphPassGenerator* eachGenerator : mAllPassGenerator)
		{
			eachGenerator->ClearRoQueue();
		}
	}

	void SceneRenderPipeline::InitNewFrameGraphGeneratorInstance(FrameGraphPassGenerator* newGenerator)
	{
		mAllPassGenerator.push_back(newGenerator);
	}

	class RENDER_API SceneRenderer
	{
		FrameGraphBuilder mFrameGraphBuilder;

		SceneRenderPipeline mRenderPipeline;

		LArray<RenderView*> mAllViews;
	public:
		SceneRenderer() :
			mFrameGraphBuilder("SceneRenderer") {};

		void Render(RenderScene* renderScene);
	private:

		void PrepareSceneRender(RenderScene* renderScene);

		void GeneratePassByView(RenderScene* renderScene,RenderView* curView);

	};

	void SceneRenderer::PrepareSceneRender(RenderScene* renderScene)
	{
		for (auto data : renderScene->mDatas)
		{
			data->PerSceneUpdate(renderScene);
		}

		for (RenderView* curView : mAllViews)
		{
			for (auto data : curView->mDatas)
			{
				data->PerViewUpdate(curView);
			}
			curView->PrepareView();
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
		mRenderPipeline.GeneratePass(&mFrameGraphBuilder, renderScene, curView);
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