#pragma once
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Graphics/Renderer/RenderData.h"

#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"

#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/FrameGraph/FrameGraph.h"

#include <functional>



namespace luna::graphics
{


enum class RenderViewType
{
	SceneView,
	ShadowMapView
};

class RENDER_API RenderView : public RenderDataContainer, public HoldIdItem
{
public:
	RenderView();
	~RenderView() = default;
	
public:
	void Culling(RenderScene* scene);
	void Render(RenderScene* scene, FrameGraphBuilder* FG);
	void PrepareView();
	

public:
	RenderTarget* GetRenderTarget() const { return mRT.Get(); }
	//ROArray& GetViewVisibleROs() { return mViewVisibleROs; }
	const LMatrix4f& GetViewMatrix() const { return mViewMatrix; }
	const LMatrix4f& GetProjectionMatrix() const { return mProjMatrix; }

	void SetRenderTarget(RenderTarget* val) { mRT = val; }
	void SetViewMatrix(const LMatrix4f& val) { mViewMatrix = val; }
	void SetViewPosition(const LVector3f& val) { mViewPos = val; }
	void SetProjectionMatrix(const LMatrix4f& val) { mProjMatrix = val; }

	RenderViewType mViewType   = RenderViewType::SceneView;

	RenderScene*   mOwnerScene = nullptr;
	ShaderCBuffer* mViewBuffer = nullptr;
private:
	//ROArray             mViewVisibleROs;
	float               mNear = 0.1f;
	float               mFar  = 1000.0f;

	LMatrix4f           mViewMatrix;
	LMatrix4f           mProjMatrix;
	LVector3f           mViewPos;

	TPPtr<RenderTarget> mRT;
};


}
