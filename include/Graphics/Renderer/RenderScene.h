#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Core/Foundation/Misc.h"
#include "Graphics/Renderer/RenderData.h"

#include <functional>


namespace luna::graphics
{

struct RENDER_API RenderObject
{
	RenderMeshBase*   mMeshIndex         = nullptr;
	int32_t           mSkinClusterIndex  = -1;
	int32_t           mAnimInstanceIndex = -1;
	LMatrix4f*        mWorldMat          = nullptr;
	bool              mCastShadow        = true;
	bool              mReceiveLight      = true;
	bool              mReceiveShadow     = true;
	uint64_t          mID                = -1;
	MaterialInstance* mMaterial          = nullptr;
	RenderScene*      mOwnerScene        = nullptr;
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


class RENDER_API RenderScene final : public RenderDataContainer
{
public:
	RenderScene();
	virtual ~RenderScene();

	void Render(FrameGraphBuilder* FG);
public:
	
	void Init();

	DirectionLight* CreateMainDirLight();
	void DestroyMainDirLight(DirectionLight* val);
	PointLight* CreatePointLight();

	uint64_t CreateRenderObject(MaterialInstance* mat, SubMesh* meshData, bool castShadow, LMatrix4f* worldMat);

	uint64_t CreateRenderObjectDynamic(
		MaterialInstance* mat,
		SubMesh* meshData,
		const LUnorderedMap<LString, int32_t>& skeletonId,
		const LString& skeletonUniqueName,
		const LString& animaInstanceUniqueName,
		const LArray<LMatrix4f>& allBoneMatrix,
		bool castShadow,
		LMatrix4f* worldMat
	);
	
	void SetRenderObjectMesh(uint64_t roId,SubMesh* meshData);

	void SetRenderObjectMeshSkletonCluster(uint64_t roId, SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName);

	void SetRenderObjectAnimInstance(uint64_t roId, const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix);

	void UpdateRenderObjectAnimInstance(uint64_t roId, const LArray<LMatrix4f>& allBoneMatrix);

	void SetRenderObjectCastShadow(uint64_t roId, bool castShadow);

	void SetRenderObjectTransformRef(uint64_t roId, LMatrix4f* worldMat);

	void SetRenderObjectMaterial(uint64_t roId, MaterialInstance* mat);

	RenderView* CreateRenderView();
	void DestroyLight(Light* ro);
	void DestroyRenderObject(uint64_t ro);
	void DestroyRenderView(RenderView* renderView);

	bool mRenderable = true;
	bool mDrawGizmos = true;

	RenderMeshBase  mDebugMeshLineData;
	RenderMeshBase  mDebugMeshData;
public:
	void        SetSceneBufferDirty()             { mBufferDirty = true; }
	RenderView* GetRenderView(uint32_t idx) const { return mViews[idx]; };
	size_t      GetRenderObjectsCount() const     { return mRenderObjects.size(); }
	size_t      GetRenderViewNum() const          { return mViews.size(); }
	auto&       GetRenderObjects() const          { return mRenderObjects; };
	auto&       GetAllView() const                { return mViews; };
	auto&		GetDirtyRos() {return mDirtyROs;}


	ShaderCBuffer* mSceneParamsBuffer    = nullptr;
	ShaderCBuffer* mROIDInstancingBuffer = nullptr;

	//Gizmos Mesh
	//SubMesh*            mDebugMeshLine        = nullptr;
	//SubMesh*            mDebugMesh            = nullptr;

	//先不做Culling，这里应该交给View进行Culling并进行ID更新
	
	RHIResourcePtr      mIDInstanceBuffer;

	//Main方向光
	DirectionLight*     mMainDirLight   = nullptr;
	//Point Lights
	LArray<PointLight*> mPointLights;
	//环境光
	LVector4f           mAmbientColor   = LVector4f(0.05, 0.05, 0.05, 1.0);
	//Skybox
	MaterialInstance*   mSkyboxMaterial = nullptr;
	
protected:
	void PrepareScene();
	void Debug();
private:
	bool                  mBufferDirty = true;
	bool                  mInit        = false;

	LQueue<uint64_t>      mRoIndex;
	LArray<RenderObject*> mRenderObjects;
	ViewArray             mViews;

	LSet<RenderObject*>   mDirtyROs;
};
}
