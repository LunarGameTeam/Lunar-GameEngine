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
		for (FrameGraphPassGenerator* eachGenerator : mAllPassGenerator)
		{
			eachGenerator->AddPassNode(frameGraph, curView, curScene);
		}
	}

	void SceneRenderPipeline::InitNewFrameGraphGeneratorInstance(FrameGraphPassGenerator* newGenerator)
	{
		mAllPassGenerator.push_back(newGenerator);
	}

	class RENDER_API SceneRenderer
	{
		RenderScene* mRenderScene = nullptr;

		FrameGraphBuilder mFrameGraphBuilder;

		SceneRenderPipeline mRenderPipeline;

	public:
		SceneRenderer(RenderScene* ownerScene) :
			mRenderScene(ownerScene),
			mFrameGraphBuilder("SceneRenderer") {};

		void Render();
	private:

		void PrepareSceneRender();

		void GeneratePassByView(RenderView* curView);

	};

	void SceneRenderer::PrepareSceneRender()
	{
		//if (mMainDirLight)
		//{
		//	if (mMainDirLight->mDirty)
		//		mBufferDirty = true;
		//	mMainDirLight->PerSceneUpdate(this);
		//}
		//for (int i = 0; i < mPointLights.size(); i++)
		//{
		//	PointLight* light = mPointLights[i];
		//	if (light->mDirty)
		//		mBufferDirty = true;
		//	light->PerSceneUpdate(this);
		//}

		for (auto data : mRenderScene->mDatas)
		{
			data->PerSceneUpdate(mRenderScene);
		}

		//PrepareScene();

		size_t viewNum = mRenderScene->GetRenderViewNum();
		for (size_t viewIndex = 0; viewIndex < viewNum; ++viewIndex)
		{
			RenderView* curView = mRenderScene->GetRenderView(viewIndex);
			for (auto data : curView->mDatas)
			{
				data->PerViewUpdate(curView);
			}
			//for (int i = 0; i < mPointLights.size(); i++)
			//{
			//	PointLight* light = mPointLights[i];
			//	light->PerViewUpdate(renderView);
			//}
			//if (mMainDirLight)
			//	mMainDirLight->PerViewUpdate(renderView);
			curView->PrepareView();
		}

		//Debug();

		//mSceneParamsBuffer->Commit();
		//mROIDInstancingBuffer->Commit();

		//for (RenderView* renderView : mViews)
		//{
		//	renderView->Culling(mRenderScene);
		//	switch (mViewType)
		//	{
		//	case RenderViewType::SceneView:
		//	{
		//		PBRPreparePass(FG, this, scene);
		//		DirectionalLightShadowPass(FG, this, scene);
		//		PointShadowPass(FG, this, scene);
		//		OpaquePass(FG, this, scene);
		//		PostProcessPass(FG, this, scene);
		//		OverlayPass(FG, this, scene);
		//		break;
		//	}
		//	case RenderViewType::ShadowMapView:
		//	{
		//		break;
		//	}
		//	default:
		//		break;
		//	}
		//	renderView->Render(this, FG);
		//}
	}

	void SceneRenderer::GeneratePassByView(RenderView* curView)
	{
		curView->Culling(mRenderScene);
		mRenderPipeline.GeneratePass(&mFrameGraphBuilder, mRenderScene, curView);
	}

	void SceneRenderer::Render()
	{
		mFrameGraphBuilder.Clear();
		PrepareSceneRender();
		size_t viewNum = mRenderScene->GetRenderViewNum();
		for (size_t viewIndex = 0; viewIndex < viewNum; ++viewIndex)
		{
			RenderView* curView = mRenderScene->GetRenderView(viewIndex);
			GeneratePassByView(curView);
		}
		mFrameGraphBuilder.Flush();
	}
}