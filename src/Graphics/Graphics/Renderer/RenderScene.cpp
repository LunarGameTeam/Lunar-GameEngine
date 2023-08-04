﻿#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/RenderModule.h"

#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Asset/MeshAssetUtils.h"
#include "Graphics/Renderer/RenderTarget.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/MaterialParam.h"
#include "Graphics/Renderer/RenderView.h"

#include "Graphics/FrameGraph/FrameGraph.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/SkeletonSkin.h"

namespace luna::graphics
{

RenderScene::RenderScene()
{
	RequireData<SkeletonSkinData>();
}

uint64_t RenderScene::CreateRenderObject(MaterialInstance* mat, SubMesh* meshData, bool castShadow, LMatrix4f* worldMat)
{
	RenderObject* ro = new RenderObject();
	if (mRoIndex.empty())
	{
		ro->mID = mRenderObjects.size();
	}
	else
	{
		ro->mID = mRoIndex.front();
		mRoIndex.pop();
	}
	ro->mOwnerScene = this;
	ro->mID = mRenderObjects.size();
	ro->mMeshIndex = meshData->GetRenderMeshBase();
	ro->mMaterial = mat;
	ro->mWorldMat = worldMat;
	ro->mCastShadow = castShadow;
	mRenderObjects.resize(ro->mID + 1);
	mRenderObjects[ro->mID] = ro;
	mDirtyROs.insert(ro);
	return ro->mID;
}

uint64_t RenderScene::CreateRenderObjectDynamic(
	MaterialInstance* mat,
	SubMesh* meshData,
	const LUnorderedMap<LString, int32_t>& skeletonId,
	const LString& skeletonUniqueName,
	const LString& animaInstanceUniqueName,
	const LArray<LMatrix4f>& allBoneMatrix,
	bool castShadow,
	LMatrix4f* worldMat
)
{
	uint64_t newRoId = CreateRenderObject(mat, meshData, castShadow, worldMat);
	SetRenderObjectMeshSkletonCluster(newRoId, meshData, skeletonId, skeletonUniqueName);
	SetRenderObjectAnimInstance(newRoId, animaInstanceUniqueName, allBoneMatrix);
	return newRoId;
}

void RenderScene::SetRenderObjectMesh(uint64_t roId, SubMesh* meshData)
{
	mRenderObjects[roId]->mMeshIndex = meshData->GetRenderMeshBase();
}

void RenderScene::SetRenderObjectMeshSkletonCluster(uint64_t roId, SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName)
{
	mRenderObjects[roId]->mSkinClusterIndex = GetData<SkeletonSkinData>()->AddMeshSkeletonLinkClusterData(meshData, skeletonId, skeletonUniqueName);
}

void RenderScene::SetRenderObjectAnimInstance(uint64_t roId, const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix)
{
	mRenderObjects[roId]->mAnimInstanceIndex = GetData<SkeletonSkinData>()->AddAnimationInstanceMatrixData(animaInstanceUniqueName, allBoneMatrix);
}

void RenderScene::UpdateRenderObjectAnimInstance(uint64_t roId, const LArray<LMatrix4f>& allBoneMatrix)
{
	GetData<SkeletonSkinData>()->UpdateAnimationInstanceMatrixData(mRenderObjects[roId]->mAnimInstanceIndex, allBoneMatrix);
}

void RenderScene::SetRenderObjectCastShadow(uint64_t roId, bool castShadow)
{
	mRenderObjects[roId]->mCastShadow = castShadow;
}

void RenderScene::SetRenderObjectTransformRef(uint64_t roId, LMatrix4f* worldMat)
{
	mRenderObjects[roId]->mWorldMat = worldMat;
}

void RenderScene::SetRenderObjectMaterial(uint64_t roId, MaterialInstance* mat)
{
	mRenderObjects[roId]->mMaterial = mat;
	mDirtyROs.insert(mRenderObjects[roId]);
}

void RenderScene::PrepareScene()
{

	if (!mBufferDirty)
		return;
	
	uint32_t shadowmapIdx = 0;
	//todo:这里dx会出现变量被优化的情况
	if (mSceneParamsBuffer == nullptr)
		mSceneParamsBuffer = new ShaderCBuffer(sRenderModule->GetRenderContext()->GetDefaultShaderConstantBufferDesc(LString("SceneBuffer").Hash()));
	if (mROIDInstancingBuffer == nullptr)
		mROIDInstancingBuffer = new ShaderCBuffer(RHIBufferUsage::VertexBufferBit, sizeof(uint32_t) * 4 * 128);

	mSceneParamsBuffer->Set("cAmbientColor", LMath::sRGB2LinearColor(mAmbientColor));

	if (mMainDirLight)
	{
		mSceneParamsBuffer->Set("cDirectionLightColor", LMath::sRGB2LinearColor(mMainDirLight->mColor));
		mSceneParamsBuffer->Set("cLightDirection", mMainDirLight->mDirection);
		mSceneParamsBuffer->Set("cDirectionLightIndensity", mMainDirLight->mIntensity);
	}
	else
	{
		mSceneParamsBuffer->Set("cDirectionLightColor", LVector4f(0, 0, 0, 0));
	}

	mSceneParamsBuffer->Set("cPointLightsCount", mPointLights.size());
	mSceneParamsBuffer->Set("cShadowmapCount", 0);
	for (int i = 0; i < mPointLights.size(); i++)
	{
		PointLight* light = mPointLights[i];
		if (light->mCastShadow)
			mSceneParamsBuffer->Set("cShadowmapCount", 6);

		mSceneParamsBuffer->Set("cPointLights", light->mPosition, i, 0);
		mSceneParamsBuffer->Set("cPointLights", LMath::sRGB2LinearColor(light->mColor), i, 16);
		mSceneParamsBuffer->Set("cPointLights", light->mIntensity, i, 32);
	}

	for (auto& ro : mRenderObjects)
	{
		uint32_t idx = ro->mID;
		mSceneParamsBuffer->Set("cRoWorldMatrix", *ro->mWorldMat, idx);
		mROIDInstancingBuffer->Set(idx * sizeof(uint32_t) * 4, idx);
	}
	mBufferDirty = false;
}

void RenderScene::Init()
{

}

DirectionLight* RenderScene::CreateMainDirLight()
{
	mMainDirLight = new DirectionLight();
	mMainDirLight->mOwnerScene = this;
	return mMainDirLight;
}

PointLight* RenderScene::CreatePointLight()
{
	auto light = new PointLight();
	light->mOwnerScene = this;
	mPointLights.push_back(light);
	return light;
}

RenderView* RenderScene::CreateRenderView()
{
	RenderView* newView = new RenderView();
	newView->mOwnerScene = this;
	mViews.push_back(newView);
	return newView;
}

void RenderScene::Render(FrameGraphBuilder* FG)
{
	if (mMainDirLight)
	{
		if (mMainDirLight->mDirty)
			mBufferDirty = true;
		mMainDirLight->PerSceneUpdate(this);
	}
	for (int i = 0; i < mPointLights.size(); i++)
	{
		PointLight* light = mPointLights[i];
		if (light->mDirty)
			mBufferDirty = true;
		light->PerSceneUpdate(this);
	}

	for (auto data : mDatas)
	{
		data->PerSceneUpdate(this);
	}

	PrepareScene();
	
	for (RenderView* renderView : mViews)
	{
		for (int i = 0; i < mPointLights.size(); i++)
		{
			PointLight* light = mPointLights[i];
			light->PerViewUpdate(renderView);
		}
		if(mMainDirLight)
			mMainDirLight->PerViewUpdate(renderView);
		renderView->PrepareView();
	}

	Debug();

	mSceneParamsBuffer->Commit();
	mROIDInstancingBuffer->Commit();

	for (RenderView* renderView : mViews)
	{
		renderView->Render(this, FG);
	}
}

void RenderScene::DestroyMainDirLight(DirectionLight* val)
{
	if (mMainDirLight == val)
	{
		delete mMainDirLight;
		mMainDirLight = nullptr;
	}
}

void RenderScene::DestroyRenderView(RenderView* renderView)
{
	for (auto it = mViews.begin(); it != mViews.end(); ++it)
	{
		if (renderView == *it)
		{
			mViews.erase(it);
			delete renderView;
			return;
		}
	}
}

void RenderScene::DestroyLight(Light* ro)
{
	for (auto it = mPointLights.begin(); it != mPointLights.end(); ++it)
	{
		if (ro == *it)
		{
			mPointLights.erase(it);
			delete ro;
			break;
		}
	}
	mBufferDirty = true;
}

void RenderScene::DestroyRenderObject(uint64_t ro)
{
	if (ro >= mRenderObjects.size())
		return;
	auto check = mRenderObjects.at(ro);
	mRoIndex.push(check->mID);	
	delete check;
	mRenderObjects[ro] = nullptr;	
	mBufferDirty = true;
}

RenderScene::~RenderScene()
{
	for (RenderView* it : mViews)
	{
		delete it;
	}
	if (mROIDInstancingBuffer)
	{
		delete mROIDInstancingBuffer;
		mROIDInstancingBuffer = nullptr;
	}
	for (PointLight* it : mPointLights)
	{
		delete it;
	}
	if (mSceneParamsBuffer)
	{
		delete mSceneParamsBuffer;
		mSceneParamsBuffer = nullptr;
	}
	mViews.clear();
	for (auto it : mRenderObjects)
	{
		delete it;
	}
	mRenderObjects.clear();
}

void RenderScene::Debug()
{
	if (!mDrawGizmos)
		return;

	SubMesh            mDebugMeshLine;
	SubMesh            mDebugMesh;
	if (mMainDirLight && mMainDirLight->mCastShadow)
	{
		LFrustum f = LFrustum::FromOrth(0.01, 50, 30, 30);
		f.Multiple(mMainDirLight->mViewMatrix.inverse());
		AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
		AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
		AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
	}

	for (int i = 0; i < mPointLights.size(); i++)
	{
		PointLight* light = mPointLights[i];
		if (light->mCastShadow)
		{
			for (int faceIdx = 0; faceIdx < 6; faceIdx++)
			{
				LFrustum f = LFrustum::MakeFrustrum(light->mFov, light->mNear, light->mFar, light->mAspect);
				f.Multiple(light->mViewMatrix[faceIdx].inverse());

				AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
				AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
				AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
			}

		}
		AddCubeWiredToSubMesh(mDebugMeshLine, light->mPosition, LVector3f(1, 1, 1), light->mColor);
	}

	mDebugMeshData.Init(&mDebugMesh);
	mDebugMeshLineData.Init(&mDebugMeshLine);
}

}
