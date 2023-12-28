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
		void InitNewPerViewFrameGraphGeneratorInstance()
		{
			mAllPassGeneratorPerView.AddNewValueTemplate<FrameGraphPassGeneratorType>();
		};
	};

	void RENDER_API InitBasePipeline(SceneRenderPipeline* curPipeline);

	class RENDER_API SceneRenderer
	{
		FrameGraphBuilder mFrameGraphBuilder;

		SceneRenderPipeline mRenderPipeline;

		LArray<RenderView*> mAllViews;
	public:
		SceneRenderer() :
			mFrameGraphBuilder("SceneRenderer") {};

		void Render(RenderScene* renderScene);

		SceneRenderPipeline* GetSceneRenderPipeline() { return &mRenderPipeline; };
	private:

		void PrepareSceneRender(RenderScene* renderScene);

		void GeneratePassByView(RenderScene* renderScene,RenderView* curView);

	};
}