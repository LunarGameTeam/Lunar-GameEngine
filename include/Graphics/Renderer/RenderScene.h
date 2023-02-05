#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Core/Foundation/Misc.h"

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

	RHIResourcePtr    mInstanceRes;
	uint64_t          mID;
};


class RENDER_API RenderScene final : NoCopy
{
public:
	RenderScene();
	~RenderScene();;	

	void Render(FrameGraphBuilder* FG);

public:
	
	void Init();

	DirectionLight* CreateMainDirLight();
	void DestroyMainDirLight(DirectionLight* val);
	PointLight* CreatePointLight();

	RenderObject* CreateRenderObject();
	RenderView* CreateRenderView();
	void DestroyRenderObject(RenderObject* ro);
	void DestroyRenderView(RenderView* renderView);

	bool mRenderable = true;
	bool mDrawGizmos = true;
public:
	void        SetSceneBufferDirty()             { mBufferDirty = true; }
	RenderView* GetRenderView(uint32_t idx) const { return mViews[idx]; };
	size_t      GetRenderObjectsCount() const     { return mRenderObjects.size(); }
	size_t      GetRenderViewNum() const          { return mViews.size(); }
	auto&       GetRenderObjects() const          { return mRenderObjects; };
	auto&       GetAllView() const                { return mViews; };


	ShaderParamsBuffer* mSceneParamsBuffer    = nullptr;
	ShaderParamsBuffer* mROIDInstancingBuffer = nullptr;

	SubMesh*            mDebugMeshLine        = nullptr;
	SubMesh*            mDebugMesh            = nullptr;

	//先不做Culling，这里应该交给View进行Culling并进行ID更新
	RHIResourcePtr      mIDInstanceBuffer;
	DirectionLight*     mMainDirLight         = nullptr;
	LArray<PointLight*> mPointLights;
protected:
	void PrepareScene();
	void Debug();
private:
	bool            mBufferDirty = true;
	bool            mInit        = false;
	ROArray         mRenderObjects;
	ViewArray       mViews;
};
}
