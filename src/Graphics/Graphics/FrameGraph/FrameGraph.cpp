﻿#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/Renderer/RenderContext.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/RHICmdList.h"
#include "Graphics/RHI/RHIDevice.h"
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"
#include "Graphics/Renderer/RenderContext.h"

namespace luna::graphics
{

const size_t sFrameGraphBufferMaxSize = 1024 * 1024 * 32 * 8;
FrameGraphBuilder::FrameGraphBuilder(const LString& graph_name)
{
	mGraphName = graph_name;
	//Frame Graph
	{
		RHIMemoryDesc FGMemoryDesc;
		FGMemoryDesc.Type = RHIHeapType::Default;
		FGMemoryDesc.SizeInBytes = sFrameGraphBufferMaxSize;
		mFGMemory = sGlobelRenderDevice->AllocMemory(FGMemoryDesc, 3);
	}
}

FrameGraphBuilder::~FrameGraphBuilder()
{
}

FGGraphDrawNode* FrameGraphBuilder::AddGraphDrawPass(const LString& name)
{
	FGGraphDrawNode* node = new FGGraphDrawNode();
	node->SetName(name);
	mNodes.push_back(node);
	return node;
}


void FrameGraphBuilder::Clear()
{
	for (auto it : mNodes)
	{
		delete it;
	}
	mNodes.clear();
}

void FrameGraphBuilder::CleanUpVirtualMemory()
{
	mFGOffset = 0;
}

LSharedPtr<FGTexture> FrameGraphBuilder::CreateCommon2DTexture(
	const LString& name,
	uint32_t width,
	uint32_t height,
	RHITextureFormat format,
	RHIImageUsage usage
)
{
	size_t curNewId = GenerateVirtualResourceId();
	LSharedPtr<FGTexture> newTexture = MakeShared<FGTexture>(curNewId, name,width, height, format, usage,this);
	return newTexture;
}

void FrameGraphBuilder::Compile()
{
}

void FrameGraphBuilder::_Prepare()
{
	ZoneScoped;
	for (FGNode* it : mNodes)
	{
		for (auto& view : it->mVirtureResView)
		{
			RHIViewPtr rhiView = sGlobelRenderDevice->CreateView(view->mRHIViewDesc);
			view->mRHIView = rhiView;
			rhiView->BindResource(view->mVirtualRes->mExternalRes);
		}
	}
}

void FrameGraphBuilder::Flush(RHICmdList* cmdList)
{
	ZoneScoped;
	for (FGNode* it : mNodes)
	{
		for (auto& view : it->mVirtureResView)
		{
			if (view->mVirtualRes->mExternalRes == nullptr)
			{
				if (view->mVirtualRes->GetDesc().mType == ResourceType::kTexture)
				{
					FGTexture* virtualRes = static_cast<FGTexture*>(view->mVirtualRes);
					RHIResourcePtr rhiRes = FGCreateTexture(virtualRes->GetDesc());
					mPhysicResourceMap.insert({ virtualRes->mUniqueId,rhiRes });
					virtualRes->BindExternalResource(rhiRes);
				}
				else
				{
					assert(false);
				}
			}
		}
	}
	_Prepare();
	{
		ZoneScopedN("Node Execute");
		for (FGNode* baseNode : mNodes)
		{
			FGGraphDrawNode* node = static_cast<FGGraphDrawNode*>(baseNode);
			ZoneScoped;
			const char* name = node->GetName().c_str();
			ZoneName(name, node->GetName().Length());
			{
				ZoneScopedN("Device Wait");
			}			
			cmdList->BeginEvent(node->GetName());
			if (node->mRT.empty())
				continue;
			{
				ZoneScopedN("Resource Barrier");
				for (auto& view : node->mVirtureResView)
				{
					ResourceBarrierDesc barrier;
					barrier.mBaseMipLevel = 0;
					barrier.mMipLevels = 1;
					barrier.mBaseDepth = view->mRHIViewDesc.mBaseArrayLayer;
					//理论上应该View使用了Res的某个Layer，只对这个Layer进行Barrier
					//但是RHIResource里只记录了一个State，因此这里对整个Res进行Barrier
					barrier.mDepth = view->mVirtualRes->mDesc.DepthOrArraySize;
					barrier.mBarrierRes = view->mVirtualRes->mExternalRes;
					switch (view->mRHIViewDesc.mViewType)
					{
					case RHIViewType::kTexture:
						barrier.mStateBefore = barrier.mBarrierRes->mState;
						barrier.mStateAfter = ResourceState::kShaderReadOnly;
						break;
					case RHIViewType::kRenderTarget:
						barrier.mStateBefore = barrier.mBarrierRes->mState;
						barrier.mStateAfter = ResourceState::kRenderTarget;
						break;
					case RHIViewType::kDepthStencil:
						barrier.mStateBefore = barrier.mBarrierRes->mState;
						barrier.mStateAfter = ResourceState::kDepthStencilWrite;
						break;
					default:
						assert(false);
						break;
					}
					cmdList->ResourceBarrierExt(barrier);
				}
			}
			

			FGResourceView& dsView = *node->mDS;
			
			uint32_t width;
			uint32_t height;
			node->mPassDesc.mColorView.clear();
			int index = 0;
			for (FGResourceView* it : node->mRT)
			{
				FGResourceView& rtView = *it;
				width = rtView.mVirtualRes->mExternalRes->GetDesc().Width;
				height = rtView.mVirtualRes->mExternalRes->GetDesc().Height;
				assert(rtView.mRHIView);
				node->mPassDesc.mColorView.emplace_back(rtView.mRHIView);
			}

			if(node->mPassDesc.mDepths.size() > 0)
			{
				node->mPassDesc.mDepthStencilView = dsView.mRHIView;
				node->mPassDesc.mDepths[0].mDepthStencilFormat = dsView.mRHIView->mBindResource->GetDesc().Format;
			}
			
			sRenderDrawContext->BindDrawCommandPassDesc(cmdList,node->mPassDesc);
			cmdList->SetViewPort(0, 0, width, height);
			cmdList->SetScissorRects(0, 0, width, width);
			node->Execute(this, cmdList);
			sRenderDrawContext->EndRenderPass(cmdList);
			cmdList->EndEvent();
		}
	}
	
}

RHIResourcePtr FrameGraphBuilder::FGCreateTexture(const RHIResDesc& resDesc)
{
	RHIResourcePtr res = sGlobelRhiResourceGenerator->GetDeviceResourceGenerator()->CreateTextureByRhiMemory(resDesc, mFGMemory, mFGOffset);
	const MemoryRequirements& memoryReq = res->GetMemoryRequirements();
	mFGOffset += memoryReq.size;
	return res;
}

RHIResourcePtr FrameGraphBuilder::FGCreateBuffer(const RHIBufferDesc& resDesc)
{
	RHIResourcePtr res = sGlobelRhiResourceGenerator->GetDeviceResourceGenerator()->CreateBufferByRhiMemory(resDesc, mFGMemory, mFGOffset);
	const MemoryRequirements& memoryReq = res->GetMemoryRequirements();
	mFGOffset += memoryReq.size;
	return res;
}
//FGTexture* FrameGraphBuilder::GetTexture(const LString& name)
//{
//	auto it = mVirtualRes.find(name);
//	if (it == mVirtualRes.end())
//		return nullptr;
//	FGTexture* texture = static_cast<FGTexture*>(it->second);
//	return texture;
//}

size_t FrameGraphBuilder::GenerateVirtualResourceId()
{
	size_t curIndex;
	if (!mUnusedVirtualResourceId.empty())
	{
		curIndex = *mUnusedVirtualResourceId.begin();
		mUnusedVirtualResourceId.erase(curIndex);
		return curIndex;
	}
	curIndex = mMaxVirtualResourceId++;
	return curIndex;
}

void FrameGraphBuilder::RemoveVirtualResourceId(size_t virtualResourceId)
{
	if (mUnusedVirtualResourceId.find(virtualResourceId) == mUnusedVirtualResourceId.end())
	{
		mUnusedVirtualResourceId.insert(virtualResourceId);
	}
	else
	{
		assert(false);
	}
}

}
