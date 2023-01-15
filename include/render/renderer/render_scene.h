#pragma once
#include "core/core_min.h"

#include "render/render_config.h"
#include "render/forward_types.h"
#include "render/rhi/rhi_types.h"

#include <functional>

namespace luna::render
{

struct RENDER_API RenderObject
{
	SubMesh* mMesh;
	MaterialInstance* mMaterial;
	LMatrix4f* mWorldMat;

	bool              mCastShadow = true;
	bool              mReceiveLight = true;
	bool              mReceiveShadow = true;

	uint64_t          mID;
};

struct RENDER_API RenderLight
{
	LVector4f mColor = LVector4f(1, 1, 1, 1);
	bool      mCastShadow;
	float     mIndensity;
	LVector3f mDirection;
	LMatrix4f mViewMatrix;
	LMatrix4f mProjMatrix;
};

class RENDER_API RenderScene
{
public:
	RenderScene();
	~RenderScene() {};

	void Render(FrameGraphBuilder* FG);

public:
	RenderLight* CreateMainDirLight();
	RenderObject* CreateRenderObject();
	RenderView* CreateRenderView();
	void DestroyRenderObject(RenderObject* ro);
	void DestroyRenderView(RenderView* renderView);

public:
	void        SetSceneBufferDirty()             { mBufferDirty = true; }
	RenderView* GetRenderView(uint32_t idx) const { return mViews[idx]; };
	size_t      GetRenderObjectsCount() const     { return mRenderObjects.size(); }
	size_t      GetRenderViewNum() const          { return mViews.size(); }
	auto&       GetRenderObjects() const          { return mRenderObjects; };
	auto&       GetAllView() const                { return mViews; };


	RHIResourcePtr mROBuffer;
	RHIViewPtr     mROBufferView;

	RHIResourcePtr mSceneBuffer;
	RHIViewPtr     mSceneBufferView;

	SubMesh*       mDebugMeshLine;
	SubMesh*       mDebugMesh;

protected:
	void CommitSceneBuffer();

private:
	bool                 mBufferDirty = true;
	bool                 mInit        = false;
	RenderLight*         mDirLight;
	LArray<RenderLight*> mLights;
	ROArray              mRenderObjects;
	ViewArray            mViews;
};
}
