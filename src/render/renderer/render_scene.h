#pragma once
#include "core/foundation/container.h"
#include "core/foundation/string.h"

#include "render/render_config.h"
#include "render/renderer/render_target.h"
#include "render/renderer/render_obj.h"
#include "render/renderer/render_view.h"

#include "render/frame_graph/frame_graph.h"
#include <functional>

namespace luna::render
{



struct RENDER_API RenderObject
{
	SubMesh*          mMesh;
	MaterialInstance* mMaterial;
	LMatrix4f*        mWorldMat;

	bool              mCastShadow    = true;
	bool              mReceiveLight  = true;
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

	inline void SetSceneBufferDirty() { mBufferDirty = true; }
	
	RenderView* GetRenderView(const int32_t view_id) const { return mViews[view_id]; };

	size_t GetRenderObjectsCount()const { return mRenderObjects.size(); }
	size_t GetRenderViewNum()const { return mViews.size(); }

	const LArray<RenderObject*>& GetRenderObjects() const { return mRenderObjects; };
	const LArray<RenderView*>& GetAllView() const { return mViews; };

	
	RHIResourcePtr mROBuffer;
	RHIViewPtr     mROBufferView;

	RHIResourcePtr mSceneBuffer;
	RHIViewPtr     mSceneBufferView;

protected:
	void CommitSceneBuffer();

private:
	bool                  mBufferDirty = true;
	bool                  mInit        = false;
	RenderLight*          mDirLight;
	LArray<RenderLight>   mLights;
	LArray<RenderObject*> mRenderObjects;
	LArray<RenderView*>   mViews;
};
}
