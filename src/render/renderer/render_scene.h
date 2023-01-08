#pragma once
#include "core/foundation/container.h"
#include "core/foundation/string.h"

#include "render/render_config.h"

#include "render/renderer/render_target.h"
#include "render/renderer/render_scene.h"
#include "render/renderer/render_view.h"

#include "render/frame_graph/frame_graph.h"
#include <functional>

namespace luna::render
{

class MaterialInstance;
class SubMesh;



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

using ROArray = LArray<RenderObject*>;
using ViewArray = LArray<RenderView*>;

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
