#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Core/Foundation/Misc.h"
#include "Graphics/Renderer/RenderData.h"
#include "Graphics/Renderer/SkeletonSkin.h"

#include <functional>


namespace luna::graphics
{

class RENDER_API RenderObject : public RenderDataContainer, public HoldIdItem
{
	LMatrix4f    mWorldMat;
	RenderScene* mOwnerScene = nullptr;
public:
	RenderObject(RenderScene* ownerScene);

	void UpdateWorldMatrix(const LMatrix4f &worldMat) { mWorldMat = worldMat; }

	const LMatrix4f& GetWorldMatrix() { return mWorldMat; }
};

class RenderObjectDrawData : public RenderData
{
public:
	using ROFilterFunc = std::function<bool(RenderObject*)>;

	LArray<RenderObject*> mVisibleROs[MeshRenderPass::AllNum];
	MaterialInstance*     mOverrideMaterialInst[MeshRenderPass::AllNum] = { nullptr };
	ROFilterFunc          mFilters[MeshRenderPass::AllNum];
	RenderScene*          mScene                                        = nullptr;
	RenderView*           mView                                         = nullptr;

	void DrawRenderObjects(MeshRenderPass pass, const std::unordered_map<luna::graphics::ShaderParamID, luna::graphics::RHIView*>& shaderBindingParam, RHIView* overrideRenderViewBuffer);

	void SetROFilter(MeshRenderPass pass, ROFilterFunc func)
	{
		mFilters[pass] = func;
	}
	void SetOverrideMaterialInstance(MeshRenderPass pass, MaterialInstance* mat)
	{
		mOverrideMaterialInst[pass] = mat;
	}
	void PerViewUpdate(RenderView* renderView) override;
};

class AmbientAtmosphereData : public RenderData
{
	//环境光
	LVector4f           mAmbientColor = LVector4f(0.05, 0.05, 0.05, 1.0);
	//Skybox
	MaterialInstance*   mSkyboxMaterial = nullptr;
};

class RenderSceneDebugData : public RenderData
{
	RenderMeshBase  mDebugMeshLineData;
	RenderMeshBase  mDebugMeshData;
};

class RENDER_API RenderScene final : public RenderDataContainer
{
public:
	RenderScene();

	virtual ~RenderScene();

	RenderObject* CreateRenderObject();

	void DestroyRenderObject(RenderObject* ro);

	void GetRenderObjects(LArray<RenderObject*>& valueOut) const;

	RenderView* CreateRenderView();

	void DestroyRenderView(RenderView* renderView);

	void GetAllView(LArray<RenderView*>& valueOut) const;
	//void Debug();
private:
	//bool                  mBufferDirty = true;
	//bool                  mInit        = false;

	LHoldIdArray<RenderObject> mRenderObjects;

	LHoldIdArray<RenderView> mViews;
};



}
