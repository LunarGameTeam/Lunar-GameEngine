#pragma once
#include "render/pch.h"
#include "render/renderer/render_target.h"
#include "render/renderer/render_obj.h"
#include "core/misc/container.h"
#include "core/misc/string.h"
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
	void Init();
	void Render(FrameGraphBuilder* FG);


public:
	void UpdateRenderObject();

	Light* AddMainLight();

	RenderObject* CreateRenderObject();
	RenderView* CreateRenderView();

	Light& GetMainDirLight() { return mDirLight; };
	const int32_t GetSceneId()const { return mSceneID; }
	const LVector<RenderObject*>& GetRenderObjects()const { return mRenderObjects; };

	const size_t GetRenderViewNum()const { return mViews.Size(); }

	const TSubPtrArray<RenderView>& GetAllView()const { return mViews; };

	RenderView* GetRenderView(const int32_t view_id)const { return mViews[view_id]; };
	RHIResourcePtr									 mROBuffer;
	RHIViewPtr										 mROBufferView;

	RHIResourcePtr									 mSceneBuffer;
	RHIViewPtr										 mSceneBufferView;

private:
	bool mInit = false;
	int32_t                                          mSceneID;
	Light                                            mDirLight;
	LVector<Light>                                   mAllLight;
	LVector<RenderObject*>							 mRenderObjects;
	TSubPtrArray<RenderView>                         mViews;
};
}
