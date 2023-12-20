#pragma once

#include "Graphics/Renderer/RenderData.h"
#include "Core/Foundation/Container.h"
#include "Core/Math/Math.h"
#include "Graphics/RenderAssetManager/RenderAssetManager.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/RenderModule.h"
#include "Graphics/Asset/MeshAssetUtils.h"
namespace luna::graphics
{
	void RenderAssetDataMesh::Init(SubMesh* meshData)
	{
		if (meshData->mVertexData.size() == 0 || meshData->mIndexData.size() == 0)
			return;
		RHIBufferDesc desc;
		desc.mBufferUsage = RHIBufferUsage::VertexBufferBit;
		mVertexSize = meshData->mVertexData.size();
		mVeretexLayout = meshData->GetVertexLayout();
		if (meshData->mType == SubMeshType::SubMeshSkined)
		{
			SubMeshSkeletal* meshSkeletalData = static_cast<SubMeshSkeletal*>(meshData);
			LArray<SkinRenderVertex> combineData;
			for (int32_t vertId = 0; vertId < mVertexSize; ++vertId)
			{
				combineData.emplace_back();
				combineData.back().mBaseValue = meshSkeletalData->mVertexData[vertId];
				combineData.back().mSkinValue = meshSkeletalData->mSkinData[vertId];
			}
			desc.mSize = sizeof(SkinRenderVertex) * mVertexSize;
			mVB = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Default, desc, combineData.data(), desc.mSize);
		}
		else
		{
			desc.mSize = sizeof(BaseVertex) * mVertexSize;
			mVB = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Default, desc, meshData->mVertexData.data(), desc.mSize);
		}
		mIndexSize = meshData->mIndexData.size();
		desc.mSize = sizeof(uint32_t) * mIndexSize;
		desc.mBufferUsage = RHIBufferUsage::IndexBufferBit;
		mIB = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Default, desc, meshData->mIndexData.data(), desc.mSize);
	}

	RenderAssetDataMesh* RenderAssetDataManager::GenerateRenderMesh(SubMesh* meshData)
	{
		LString pathStr = meshData->mAssetPath;
		pathStr + ":_*sub*_:" + std::to_string(meshData->GetIndexCount());
		RenderAssetDataMesh* newData = mAllRenderMesh.AddNewValue();
		newData->Init(meshData);
		return newData;
	}
	
	RenderAssetDataManager::RenderAssetDataManager()
	{

	};
	
	RenderAssetDataMesh* RenderAssetDataManager::GetFullScreenMesh()
	{
		if (mFullScreenRenderMesh == nullptr)
		{
			SubMesh fullscreenMeshAsset;
			BaseVertex v1, v2, v3;
			v1.pos = LVector3f(-1, -1, 0);
			v1.uv[0] = LVector2f(0, 1);
			v2.pos = LVector3f(-1, 1, 0);
			v2.uv[0] = LVector2f(0, 0);
			v3.pos = LVector3f(1, 1, 0);
			v3.uv[0] = LVector2f(1, 0);
			AddTriangleToSubMesh(v1, v2, v3, fullscreenMeshAsset);
			v1.pos = LVector3f(1, 1, 0);
			v1.uv[0] = LVector2f(1, 0);
			v2.pos = LVector3f(1, -1, 0);
			v2.uv[0] = LVector2f(1, 1);
			v3.pos = LVector3f(-1, -1, 0);
			v3.uv[0] = LVector2f(0, 1);
			AddTriangleToSubMesh(v1, v2, v3, fullscreenMeshAsset);
			mFullScreenRenderMesh = GenerateRenderMesh(&fullscreenMeshAsset);
		}
		return mFullScreenRenderMesh;
	}

	RenderAssetDataMesh* RenderAssetDataManager::GetDebugMeshLineMesh()
	{
		if (mDebugMeshLineData == nullptr)
		{
			SubMesh            debugMeshLine;
			LFrustum f = LFrustum::FromOrth(0.01, 50, 30, 30);
			f.Multiple(LMatrix4f::Identity());
			AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], debugMeshLine);
			mDebugMeshLineData = GenerateRenderMesh(&debugMeshLine);
		}
		return mDebugMeshLineData;
	}

	RenderAssetDataMesh* RenderAssetDataManager::GetDebugMesh()
	{
		if (mDebugMeshData == nullptr)
		{
			SubMesh            debugMeshLine;
			LFrustum f = LFrustum::FromOrth(0.01, 50, 30, 30);
			f.Multiple(LMatrix4f::Identity());
			AddLineToSubMesh(f.mNearPlane[0], f.mNearPlane[1], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[1], f.mNearPlane[2], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[2], f.mNearPlane[3], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[3], f.mNearPlane[0], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[0], f.mFarPlane[0], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[1], f.mFarPlane[1], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[2], f.mFarPlane[2], debugMeshLine);
			AddLineToSubMesh(f.mNearPlane[3], f.mFarPlane[3], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[0], f.mFarPlane[1], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[1], f.mFarPlane[2], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[2], f.mFarPlane[3], debugMeshLine);
			AddLineToSubMesh(f.mFarPlane[3], f.mFarPlane[0], debugMeshLine);
			mDebugMeshData = GenerateRenderMesh(&debugMeshLine);
		}
		return mDebugMeshLineData;
	}
	
	void RenderAssetDataManager::DestroyRenderMesh(RenderAssetDataMesh* newData)
	{
		mAllRenderMesh.DestroyValue(newData);
	}
}
