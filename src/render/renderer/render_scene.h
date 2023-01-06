#pragma once
#include "render/render_config.h"
#include "render/renderer/render_target.h"
#include "render/renderer/render_obj.h"
#include "core/foundation/container.h"
#include "core/foundation/string.h"
#include "render/frame_graph/frame_graph.h"
#include <functional>
#include"render_light.h"
#include"render_view.h"
namespace luna::render
{


class RENDER_API RenderScene :public LObject
{
public:
	RenderScene(int32_t scene_id);
	~RenderScene() = default;

	void Render(FrameGraphBuilder* FG);

public:
	Light* CreateMainDirLight();
	RenderObject* CreateRenderObject();
	RenderView* CreateRenderView();

	inline void SetSceneBufferDirty() { mBufferDirty = true; }
	
	const LArray<RenderObject*>& GetRenderObjects() const { return mRenderObjects; };

	size_t GetRenderViewNum()const { return mViews.Size(); }

	const TPPtrArray<RenderView>& GetAllView() const { return mViews; };

	RenderView* GetRenderView(const int32_t view_id)const { return mViews[view_id]; };

	RHIResourcePtr mROBuffer;
	RHIViewPtr     mROBufferView;

	RHIResourcePtr mSceneBuffer;
	RHIViewPtr     mSceneBufferView;

protected:
	void CommitSceneBuffer();

private:
	bool                     mBufferDirty = true;
	bool                     mInit        = false;
	int32_t                  mSceneID;
	Light*                   mDirLight;
	LArray<Light>           mAllLight;
	LArray<RenderObject*>   mRenderObjects;
	TPPtrArray<RenderView> mViews;
};
}
