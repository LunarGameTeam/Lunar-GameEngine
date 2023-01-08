#pragma once
#include "render/render_config.h"

#include "core/foundation/container.h"
#include "core/foundation/string.h"


#include "render/renderer/render_target.h"

#include "render/frame_graph/frame_graph.h"

#include <functional>
#include "render/renderer/render_pass.h"


namespace luna::render
{

struct RenderObject;
class RenderScene;
class RenderModule;

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

	void GenerateSceneViewPass(RenderScene* scene,FrameGraphBuilder* FG);
	void GenerateShadowViewPass(RenderScene* scene, FrameGraphBuilder* FG);

	void PrepareViewBuffer();

public:
	FORCEINLINE uint64_t GetViewID()const { return mViewID; }
	FORCEINLINE RenderTarget* GetRenderTarget() const { return mRT.Get(); }
	FORCEINLINE const LMatrix4f& GetViewMatrix() const { return mViewMatrix; }
	FORCEINLINE const LMatrix4f& GetProjectionMatrix() const { return mProjMatrix; }

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
	float mNear = 0.1f;
	float mFar = 1000.0f;

	RenderPass* mDebugPass;
	RenderPass* mLightingPass;

	RHIResourcePtr mPerViewBuffer;
	RHIViewPtr mPerViewBufferView;

	LMatrix4f mViewMatrix;
	LMatrix4f mProjMatrix;
	LVector3f mViewPos;

	uint64_t mViewID;
	TPPtr<RenderTarget> mRT;
};


}
