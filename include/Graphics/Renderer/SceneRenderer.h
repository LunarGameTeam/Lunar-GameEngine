#pragma once
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/FrameGraph/FrameGraphPassGenerator.h"
#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/Renderer/RenderView.h"
namespace luna::graphics
{
	class RENDER_API SceneRenderPipeline
	{
		LHoldIdArray<FrameGraphPassGeneratorPerView> mAllPassGeneratorPerView;
	public:
		void GeneratePerViwewPass(FrameGraphBuilder* frameGraph, RenderView* curView);
		template<typename FrameGraphPassGeneratorType>
		void InitNewPerViewFrameGraphGeneratorInstance();
	};

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

	template<typename FrameGraphPassGeneratorType>
	void SceneRenderPipeline::InitNewPerViewFrameGraphGeneratorInstance()
	{
		mAllPassGeneratorPerView.AddNewValueTemplate<FrameGraphPassGeneratorType>();
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