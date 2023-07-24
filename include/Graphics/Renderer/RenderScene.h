﻿#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Core/Foundation/Misc.h"
#include "Graphics/Renderer/RenderData.h"

#include <functional>

namespace luna::render
{
template<typename DataType>
class AssetSceneMemberDataPack
{
	LQueue<size_t> mEmptyId;
	LUnorderedMap<LString, int32_t> mDataName;
	LUnorderedMap<int32_t, LSharedPtr<DataType>> mDataBuffer;
public:
	int32_t CheckDataIdByName(const LString& uniqueDataName)
	{
		auto itor = mDataName.find(uniqueDataName);
		if (itor == mDataName.end())
		{
			return -1;
		}
		return itor->second;
	};

	int32_t AddData(const LString &uniqueDataName)
	{
		int32_t dataIndex = CheckDataIdByName(uniqueDataName);
		if (dataIndex != -1)
		{
			return dataIndex;
		}
		if (mEmptyId.empty())
		{
			dataIndex = mDataName.size();
		}
		else
		{
			dataIndex = mEmptyId.front();
			mEmptyId.pop();
		}
		mDataName.insert({ uniqueDataName ,dataIndex });
		mDataBuffer[dataIndex] = MakeShared<DataType>();
		return dataIndex;
	}

	DataType* GetData(int32_t meshId)
	{
		auto curData = mDataBuffer.find(meshId);
		if (curData == mDataBuffer.end())
		{
			return nullptr;
		}
		return curData->second.get();
	}
};
class AssetSceneData : public RenderData
{
public:
	AssetSceneData();
	LUnorderedMap<LString, ShaderParamSceneBuffer> materialBuffer;
	//mesh现在先只添加不删除，有时间再处理ro离开场景怎么删除对应的scene资源的问题
	int32_t AddMeshData(SubMesh* meshData);

	int32_t GetMeshDataId(SubMesh* meshData);

	RenderMeshBase* GetMeshData(int32_t meshId);

	int32_t AddMeshSkeletonLinkClusterData(SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId,const LString& skeletonUniqueName);

	int32_t GetMeshSkeletonLinkClusterDataId(SubMesh* meshData, const LString& skeletonUniqueName);

	MeshSkeletonLinkClusterBase* GetMeshSkeletonLinkClusterData(int32_t clusterId);

	int32_t AddAnimationInstanceMatrixData(const LString& animaInstanceUniqueName, const LArray<LMatrix4f> &allBoneMatrix);

	void UpdateAnimationInstanceMatrixData(int32_t animInstanceId,const LArray<LMatrix4f>& allBoneMatrix);

	int32_t GetAnimationInstanceMatrixDataId(const LString& animaInstanceUniqueName);

	AnimationInstanceMatrix* GetAnimationInstanceMatrixData(int32_t animInstanceId);

	RHIView* GetSkinMatrixBuffer() { return mSkeletonResultBufferView.get(); }

public:
	void PerSceneUpdate(RenderScene* renderScene) override;

private:
	LQueue<size_t> emptyMeshId;

	AssetSceneMemberDataPack<RenderMeshBase> meshPrimitiveBuffer;
	
	AssetSceneMemberDataPack<MeshSkeletonLinkClusterBase> mMeshSkeletonLinkClusterBuffer;
	
	AssetSceneMemberDataPack<AnimationInstanceMatrix> mAnimationInstanceMatrixBuffer;

	LString GetSubmeshName(SubMesh* meshData);

	LString GetClusterName(SubMesh* meshData, const LString& skeletonUniqueName);

	RHIResourcePtr mSkeletonResultBuffer;
	RHIViewPtr     mSkeletonResultBufferView;
};

struct RENDER_API RenderObject
{
	int32_t           mMeshIndex = -1;
	int32_t           mSkinClusterIndex = -1;
	int32_t           mAnimInstanceIndex = -1;
	LMatrix4f*        mWorldMat;
	bool              mCastShadow = true;
	bool              mReceiveLight = true;
	bool              mReceiveShadow = true;
	uint64_t          mID = -1;
	MaterialInstance* mMaterial;
	LUnorderedMap<MeshRenderPass,uint64_t>  mAllCommandsIndex;
};

class RENDER_API RenderScene final : public RenderDataContainer
{
public:
	RenderScene();
	virtual ~RenderScene();

	void Render(FrameGraphBuilder* FG);
	//暂时先固定mesh pass在枚举确定，后面再看怎么拓展
	PerPassDrawCmds mAllMeshDrawCommands[MeshRenderPass::AllNum];
public:
	
	void Init();

	DirectionLight* CreateMainDirLight();
	void DestroyMainDirLight(DirectionLight* val);
	PointLight* CreatePointLight();

	uint64_t CreateRenderObject(MaterialInstance* mat, SubMesh* meshData,bool castShadow, LMatrix4f* worldMat);

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

	void UpdateRenderObject(uint64_t roId);

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
	bool            mBufferDirty = true;
	bool            mInit        = false;

	LQueue<uint64_t> mRoIndex;
	LUnorderedMap<uint64_t, RenderObject* > mRenderObjects;
	ViewArray       mViews;

	LSet<RenderObject*> mDirtyROs;
};
}
