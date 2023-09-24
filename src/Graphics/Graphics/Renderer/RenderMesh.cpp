#include "Graphics/Renderer/RenderMesh.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/Renderer/ScenePipeline.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/RenderModule.h"
namespace luna::graphics
{

void RenderMeshBase::Init(SubMesh* meshData)
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
		mVB = sRenderModule->mRenderContext->CreateBuffer(RHIHeapType::Default,desc, combineData.data(), desc.mSize);
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

}
