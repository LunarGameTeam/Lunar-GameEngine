#pragma once
#include "render/pch.h"

#include "core/misc/container.h"
#include "core/misc/string.h"


#include "render/renderer/render_target.h"

#include "render/frame_graph/frame_graph.h"

#include <functional>
#include "render/renderer/render_pass.h"


namespace luna::render
{

struct RenderObject;
class RenderScene;
class RenderModule;

enum class RenderViewType
{
	SceneView,
	CSMShadowView
};

class RENDER_API RenderView : public LObject
{
public:
	RenderView(uint64_t view_id);

	~RenderView() = default;
public:
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
	void SetProjectionMatrix(const LMatrix4f& val) { mProjMatrix = val; }

	RenderViewType mViewType = RenderViewType::SceneView;;
	float mNear;
	float mFar;

	RHIView* GetPerViewBufferView() 
	{
		return mPerViewBufferView; 
	}
private:
	RenderPass* mDebugPass;
	RenderPass* mLightingPass;

	RHIResourcePtr mPerViewBuffer;
	RHIViewPtr mPerViewBufferView;

	LMatrix4f mViewMatrix;
	LMatrix4f mProjMatrix;

	uint64_t mViewID;
	TSubPtr<RenderTarget> mRT;
};


}
