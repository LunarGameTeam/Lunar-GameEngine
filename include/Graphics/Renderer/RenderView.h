#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"

#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/FrameGraph/FrameGraph.h"

#include <functional>



namespace luna::render
{


struct PerViewBuffer
{
	LMatrix4f viewMatrix;
	LMatrix4f projectionMatrix;
	LVector2f newFar;
	LVector3f camPos;
};

enum class RenderViewType
{
	SceneView,
	ShadowMapView
};

class RENDER_API RenderView
{
public:
	RenderView(uint64_t view_id);

	~RenderView() = default;
public:
	void ScenePipeline(RenderScene* scene, FrameGraphBuilder* FG);

	void GenerateSceneViewPass(RenderScene* scene, FrameGraphBuilder* FG);
	void GenerateShadowViewPass(RenderScene* scene, FrameGraphBuilder* FG);

	void PrepareViewBuffer();

public:
	uint64_t GetViewID()const { return mViewID; }
	RenderTarget* GetRenderTarget() const { return mRT.Get(); }
	const LMatrix4f& GetViewMatrix() const { return mViewMatrix; }
	const LMatrix4f& GetProjectionMatrix() const { return mProjMatrix; }

	void SetRenderTarget(RenderTarget* val) { mRT = val; }
	void SetViewMatrix(const LMatrix4f& val) { mViewMatrix = val; }
	void SetViewPosition(const LVector3f& val) { mViewPos = val; }
	void SetProjectionMatrix(const LMatrix4f& val) { mProjMatrix = val; }

	RenderViewType mViewType = RenderViewType::SceneView;


	RHIView* GetPerViewBufferView()
	{
		return mPerViewBufferView;
	}

private:
	float               mNear = 0.1f;
	float               mFar  = 1000.0f;

	RHIResourcePtr      mPerViewBuffer;
	RHIViewPtr          mPerViewBufferView;

	LMatrix4f           mViewMatrix;
	LMatrix4f           mProjMatrix;
	LVector3f           mViewPos;

	uint64_t            mViewID;
	TPPtr<RenderTarget> mRT;
};


}
