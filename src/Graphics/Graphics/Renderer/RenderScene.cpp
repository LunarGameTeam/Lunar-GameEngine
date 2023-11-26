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

RenderSceneStagingMemory::RenderSceneStagingMemory(size_t perStageSize = 0x800000) : mPerStageSize(perStageSize)
{
	AddNewMemory(perStageSize);
	mPoolUsedSize = 0;
}

RHIResource* RenderSceneStagingMemory::AllocStructStageBuffer(size_t curBufferSize)
{
	//目前不考虑多线程访问的问题，默认render只有一个线程
	RHIResource* curResourceItm = nullptr;
	if (mPoolUsedSize < mStageBufferPool.size())
	{
		RHIBufferDesc newBufferDesc;
		newBufferDesc.mBufferUsage = RHIBufferUsage::TransferSrcBit | RHIBufferUsage::StructureBuffer;
		newBufferDesc.mSize = SizeAligned2Pow(curBufferSize, CommonSize64K);
		RHIResourcePtr newBuffer = sRenderModule->GetRenderContext()->mDevice->CreateBufferExt(newBufferDesc);
		mStageBufferPool.push_back(newBuffer);
	}
	curResourceItm = mStageBufferPool[mPoolUsedSize].get();
	BindMemory(curResourceItm);
	mPoolUsedSize += 1;
	return curResourceItm;
};

RHIResource* RenderSceneStagingMemory::AllocUniformStageBuffer(ShaderCBuffer* cbufferData)
{
	RHIResource* newRes = AllocStructStageBuffer(cbufferData->mData.size());
	void* cpuPointer = newRes->Map();
	memcpy(cpuPointer, cbufferData->mData.data(), cbufferData->mData.size());
	newRes->Unmap();
	return newRes;
}

void RenderSceneStagingMemory::AddNewMemory(size_t memorySize)
{
	RHIMemoryPtr curMemoryData;
	RHIMemoryDesc memoryDesc;
	memoryDesc.SizeInBytes = memorySize;
	memoryDesc.Type = RHIHeapType::Upload;
	mMemoryOffset = 0;
	curMemoryData = sRenderModule->GetRenderContext()->mDevice->AllocMemory(memoryDesc);
	mMemoryData.push_back(curMemoryData);
	mMemoryDataSize.push_back(memorySize);
}

bool RenderSceneStagingMemory::BindMemory(RHIResource* targetResource)
{
	size_t mMemorySize = mMemoryDataSize.back();
	size_t reauiredSize = SizeAligned2Pow(targetResource->GetMemoryRequirements().size, CommonSize64K);
	if ((mMemoryOffset + reauiredSize + CommonSize64K) >= mMemorySize)
	{
		if (reauiredSize * 4 >= mPerStageSize)
		{
			size_t nextBufferSize = SizeAligned2Pow(reauiredSize * 4, CommonSize64K);
			AddNewMemory(nextBufferSize);
		}
		else
		{
			AddNewMemory(mPerStageSize);
		}
		mMemoryOffset = 0;
	}
	targetResource->BindMemory(mMemoryData.back().get(), mMemoryOffset);
	mMemoryOffset += reauiredSize;
	return true;
}

void RenderSceneStagingMemory::ClearMemory()
{
	if (mMemoryData.size() == 1)
	{
		mMemoryOffset = 0;
		return;
	}
	size_t nextFrameSize = 0;
	for (int32_t memoryIndex = 0; memoryIndex < mMemoryDataSize.size() - 1; ++memoryIndex)
	{
		nextFrameSize += mMemoryDataSize[memoryIndex];
	}
	nextFrameSize += mMemoryOffset;
	nextFrameSize += 0x400000;
	mMemoryOffset = 0;
	mMemoryData.clear();
	mMemoryDataSize.clear();
	AddNewMemory(nextFrameSize);
};

RenderSceneUploadBufferPool::RenderSceneUploadBufferPool()
{
	mFrameIndex = 0;
	for (int32_t i = 0; i < 2; ++i)
	{
		mMemoryArray.emplace_back();
	}

}

RHIResource* RenderSceneUploadBufferPool::AllocStructStageBuffer(size_t curBufferSize)
{
	return mMemoryArray[mFrameIndex].AllocStructStageBuffer(curBufferSize);
}

RHIResource* RenderSceneUploadBufferPool::AllocUniformStageBuffer(ShaderCBuffer* cbufferData)
{
	return mMemoryArray[mFrameIndex].AllocUniformStageBuffer(cbufferData);
}

void RenderSceneUploadBufferPool::Present()
{
	mFrameIndex = (mFrameIndex + 1) % 2;
}

PARAM_ID(RoWorldMatrixBuffer);
RenderScene::RenderScene()
{
	RequireData<SkeletonSkinData>();
	//if (mRoDataBuffer == nullptr)
	//{
	//	RHIBufferDesc desc;
	//	desc.mBufferUsage = RHIBufferUsage::StructureBuffer;
	//	desc.mSize = sizeof(LMatrix4f) * 1024;
	//	mRoDataBuffer = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Upload, desc);
	//	ViewDesc viewDesc;
	//	viewDesc.mViewType = RHIViewType::kStructuredBuffer;
	//	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	//	viewDesc.mStructureStride = sizeof(LMatrix4f);
	//	mRoDataBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	//	mRoDataBufferView->BindResource(mRoDataBuffer);
	//}
}

//void RenderScene::SetMaterialSceneParameter(MaterialInstance* matInstance)
//{
//	matInstance->SetShaderInput(ParamID_RoWorldMatrixBuffer, mRoDataBufferView);
//}

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

GpuSceneUploadComputeCommand* RenderScene::AddComputeCommand()
{
	mAllComputeCommand.emplace_back();
	return &mAllComputeCommand.back();
}

GpuSceneUploadCopyCommand* RenderScene::AddCopyCommand()
{
	mAllCopyCommand.emplace_back();
	return &mAllCopyCommand.back();
}

RenderSceneUploadBufferPool* RenderScene::GetStageBufferPool()
{
	return &mStageBufferPool;
}

void RenderScene::ExcuteCopy()
{
	LArray<ResourceBarrierDesc> curResourceBarrier;
	//修改资源状态
	for (auto& eachCommand : mAllComputeCommand)
	{
		for (auto& eachInput : eachCommand.mStorageBufferInput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachInput;
			newBarrierDesc.mStateBefore = ResourceState::kGenericRead;
			newBarrierDesc.mStateAfter = ResourceState::kNonPixelShaderResource;
			curResourceBarrier.push_back(newBarrierDesc);
		}
		for (auto& eachOut : eachCommand.mStorageBufferInput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachOut;
			newBarrierDesc.mStateBefore = ResourceState::kNonPixelShaderResource;
			newBarrierDesc.mStateAfter = ResourceState::kUnorderedAccess;
			curResourceBarrier.push_back(newBarrierDesc);
		}
	}
	for (auto& eachCommand : mAllCopyCommand)
	{
		ResourceBarrierDesc newInputBarrierDesc;
		newInputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
		newInputBarrierDesc.mStateBefore = ResourceState::kGenericRead;
		newInputBarrierDesc.mStateAfter = ResourceState::kCopySource;
		curResourceBarrier.push_back(newInputBarrierDesc);

		ResourceBarrierDesc newOutputBarrierDesc;
		newOutputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
		newOutputBarrierDesc.mStateBefore = ResourceState::kVertexAndConstantBuffer;
		newOutputBarrierDesc.mStateAfter = ResourceState::kCopyDest;
		curResourceBarrier.push_back(newInputBarrierDesc);
	}
	sRenderModule->mRenderContext->mBarrierCmd->GetCmdList()->ResourceBarrierExt(curResourceBarrier);
	//执行GPUscene的更新指令
	for (auto& eachCommand : mAllComputeCommand)
	{
		sRenderModule->mRenderContext->Dispatch(eachCommand.mComputeMaterial, eachCommand.mDispatchSize);
	}
	for (auto& eachCommand : mAllCopyCommand)
	{
		sRenderModule->mRenderContext->mGraphicCmd->GetCmdList()->CopyBufferToBuffer(eachCommand.mStorageBufferOutput, eachCommand.mDstOffset, eachCommand.mUniformBufferInput, eachCommand.mSrcOffset, eachCommand.mCopyLength);
	}
	//恢复资源状态
	curResourceBarrier.clear();
	for (auto& eachCommand : mAllComputeCommand)
	{
		for (auto& eachInput : eachCommand.mStorageBufferInput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachInput;
			newBarrierDesc.mStateBefore = ResourceState::kNonPixelShaderResource;
			newBarrierDesc.mStateAfter = ResourceState::kGenericRead;
			curResourceBarrier.push_back(newBarrierDesc);
		}
		for (auto& eachOut : eachCommand.mStorageBufferInput)
		{
			ResourceBarrierDesc newBarrierDesc;
			newBarrierDesc.mBarrierRes = eachOut;
			newBarrierDesc.mStateBefore = ResourceState::kUnorderedAccess;
			newBarrierDesc.mStateAfter = ResourceState::kNonPixelShaderResource;
			curResourceBarrier.push_back(newBarrierDesc);
		}
	}
	for (auto& eachCommand : mAllCopyCommand)
	{
		ResourceBarrierDesc newInputBarrierDesc;
		newInputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
		newInputBarrierDesc.mStateBefore = ResourceState::kCopySource;
		newInputBarrierDesc.mStateAfter = ResourceState::kGenericRead;
		curResourceBarrier.push_back(newInputBarrierDesc);

		ResourceBarrierDesc newOutputBarrierDesc;
		newOutputBarrierDesc.mBarrierRes = eachCommand.mUniformBufferInput;
		newOutputBarrierDesc.mStateBefore = ResourceState::kCopyDest;
		newOutputBarrierDesc.mStateAfter = ResourceState::kVertexAndConstantBuffer;
		curResourceBarrier.push_back(newInputBarrierDesc);
	}
	//情况G所有puScene的更新指令
	mAllComputeCommand.clear();
	mAllCopyCommand.clear();
	//将上传缓冲池切换到下一帧
	mStageBufferPool.Present();
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
