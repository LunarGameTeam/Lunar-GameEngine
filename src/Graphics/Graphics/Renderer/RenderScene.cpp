#include "Graphics/Renderer/RenderScene.h"
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

PARAM_ID(RoWorldMatrixBuffer);
RenderScene::RenderScene()
{
	RequireData<SkeletonSkinData>();
	if (mRoDataBuffer == nullptr)
	{
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
		desc.mSize = sizeof(LMatrix4f) * 1024;
		mRoDataBuffer = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Upload, desc);
		ViewDesc viewDesc;
		viewDesc.mViewType = RHIViewType::kStructuredBuffer;
		viewDesc.mViewDimension = RHIViewDimension::BufferView;
		viewDesc.mStructureStride = sizeof(LMatrix4f);
		mRoDataBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
		mRoDataBufferView->BindResource(mRoDataBuffer);
	}
}

void RenderScene::SetMaterialSceneParameter(MaterialInstance* matInstance)
{
	matInstance->SetShaderInput(ParamID_RoWorldMatrixBuffer, mRoDataBufferView);
}

RenderObject* RenderScene::CreateRenderObject()
{
	return mRenderObjects.AddNewValue();
}

void RenderScene::DestroyRenderObject(RenderObject* ro)
{
	mRenderObjects.DestroyValue(ro);
}

void RenderScene::GetRenderObjects(LArray<RenderObject*>& valueOut) const
{
	mRenderObjects.GetAllValueList(valueOut);
}

RenderView* RenderScene::CreateRenderView()
{
	return mViews.AddNewValue();
}

void RenderScene::GetAllView(LArray<RenderView*>& valueOut) const
{
	mViews.GetAllValueList(valueOut);
}

void RenderScene::DestroyRenderView(RenderView* renderView)
{
	mViews.DestroyValue(renderView);
}

RenderScene::~RenderScene()
{
}

//void RenderScene::Debug()
//{
//	if (!mDrawGizmos)
//		return;
//
//	SubMesh            mDebugMeshLine;
//	SubMesh            mDebugMesh;
//	if (mMainDirLight && mMainDirLight->mCastShadow)
//	{
//		LFrustum f = LFrustum::FromOrth(0.01, 50, 30, 30);
//		f.Multiple(mMainDirLight->mViewMatrix.inverse());
//		AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
//		AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
//		AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
//	}
//
//	for (int i = 0; i < mPointLights.size(); i++)
//	{
//		PointLight* light = mPointLights[i];
//		if (light->mCastShadow)
//		{
//			for (int faceIdx = 0; faceIdx < 6; faceIdx++)
//			{
//				LFrustum f = LFrustum::MakeFrustrum(light->mFov, light->mNear, light->mFar, light->mAspect);
//				f.Multiple(light->mViewMatrix[faceIdx].inverse());
//
//				AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], mDebugMeshLine);
//				AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], mDebugMeshLine);
//				AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], mDebugMeshLine);
//			}
//
//		}
//		AddCubeWiredToSubMesh(mDebugMeshLine, light->mPosition, LVector3f(1, 1, 1), light->mColor);
//	}
//
//	mDebugMeshData.Init(&mDebugMesh);
//	mDebugMeshLineData.Init(&mDebugMeshLine);
//}

}
