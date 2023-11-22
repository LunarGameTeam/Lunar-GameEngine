#pragma once

#include "Graphics/Renderer/RenderData.h"
#include "Core/Foundation/Container.h"
#include "Core/Math/Math.h"
#include "Graphics/RenderAssetManager/RenderAssetManager.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/RenderModule.h"
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
	}

	void RenderAssetDataManager::DestroyRenderMesh(RenderAssetDataMesh* newData)
	{
		mAllRenderMesh.DestroyValue(newData);
	}
}
