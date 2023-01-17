#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"

#include <functional>
#include "Core/Foundation/Misc.h"

namespace luna::render
{


struct PerObjectBuffer
{
	LMatrix4f mWorldMatrix;
};

struct SceneBuffer
{
	LVector4f       mLightDiffuseColor;
	LVector3f       mLightDirection;
};

struct InstanceBuffer
{
	uint32_t mInstance[128][4];
};

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

struct RENDER_API Light
{

	LVector4f      mColor = LVector4f(1, 1, 1, 1);
	bool           mCastShadow = false;

	float          mIndensity = 1.0;
	bool           mInit = false;
};

struct RENDER_API PointLight : Light
{
	LVector3f mPosition;
};

struct RENDER_API DirectionLight : Light
{

	LVector3f      mDirection;
	LMatrix4f      mViewMatrix;
	LMatrix4f      mProjMatrix;


	void Init();

	RHIResourcePtr mViewBuffer;
	RHIViewPtr     mViewBufferView;
};

class RENDER_API RenderScene final : NoCopy
{
public:
	RenderScene();
	~RenderScene() {};	

	void Render(FrameGraphBuilder* FG);

public:
	void Init();
	DirectionLight* CreateMainDirLight();
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


	ShaderParamsBuffer* mSceneParamsBuffer = nullptr;
	ShaderParamsBuffer* mROIDInstancingBuffer = nullptr;

	SubMesh*            mDebugMeshLine;
	SubMesh*            mDebugMesh;

	//先不做Culling，这里应该交给View进行Culling并进行ID更新
	RHIResourcePtr      mIDInstanceBuffer;
	DirectionLight*     mMainDirLight;
protected:
	void PrepareScene();

private:
	bool            mBufferDirty = true;
	bool            mInit        = false;
	ROArray         mRenderObjects;
	ViewArray       mViews;
};
}
