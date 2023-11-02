#include "Graphics/FrameGraph/FrameGraph.h"

#include "Graphics/Renderer/RenderContext.h"

#include "Graphics/RenderModule.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/RHICmdList.h"
#include "Graphics/RHI/RHIDevice.h"

#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"

#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Asset/ShaderAsset.h"


namespace luna::graphics
{

FrameGraphBuilder::FrameGraphBuilder(const LString& graph_name)
	:mFenceValue3D(sRenderModule->GetRenderContext()->mFenceValue)
{
	mGraphName = graph_name;
	mFence3D = sRenderModule->GetRenderContext()->mFence;
}

FrameGraphBuilder::~FrameGraphBuilder()
{
}

FGNode& FrameGraphBuilder::AddPass(const LString& name)
{
	FGNode* node = new FGNode();
	node->mName = name;
	mNodes.push_back(node);
	return *node;
}


void FrameGraphBuilder::Clear()
{

	for (auto it : mNodes)
	{
		delete it;
	}
	mNodes.clear();


}

LSharedPtr<FGTexture> FrameGraphBuilder::CreateCommon2DTexture(
	const LString& name,
	uint32_t width,
	uint32_t height,
	RHITextureFormat format
)
{
	size_t curNewId = GenerateVirtualResourceId();
	LSharedPtr<FGTexture> newTexture = MakeShared<FGTexture>(curNewId, width, height, format,this);
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
		for (FGResourceView* view : it->mVirtureResView)
		{
			RHIViewPtr rhiView = sRenderModule->GetRHIDevice()->CreateView(view->mRHIViewDesc);
			view->mRHIView = rhiView;
			rhiView->BindResource(view->mVirtualRes->mRes);
		}
	}
}

void FrameGraphBuilder::Flush()
{

	ZoneScoped;
	RenderContext* renderDevice = sRenderModule->GetRenderContext();
	RHISinglePoolSingleCmdList* cmdlist = renderDevice->mGraphicCmd.get();

	mFence3D->Wait(mFenceValue3D);
	for (auto& it : mVirtualRes)
	{
		if (it.second->GetRHIResource() == nullptr)
		{
			if (it.second->GetDesc().mType == ResourceType::kTexture)
			{
				FGTexture* virtualRes = static_cast<FGTexture*>(it.second);
				RHITextureDesc textureDesc;
				RHIResourcePtr rhiRes = renderDevice->FGCreateTexture(textureDesc, virtualRes->GetDesc());
				virtualRes->SetRHIResource(rhiRes);
			}
			else 
			{
				assert(false);
			}
		}

	}

	_Prepare();

	{
		ZoneScopedN("Node Execute");
		for (FGNode* node : mNodes)
		{
			ZoneScoped;
			const char* name = node->GetName().c_str();
			ZoneName(name, node->GetName().Length());
			{
				ZoneScopedN("Device Wait");
				mFence3D->Wait(mFenceValue3D);
			}			
			cmdlist->Reset();
			cmdlist->GetCmdList()->BeginEvent(node->GetName());
			if (node->mRT.empty())
				continue;
			{
				ZoneScopedN("Resource Barrier");
				for (FGResourceView* view : node->mVirtureResView)
				{
					ResourceBarrierDesc barrier;
					barrier.mBaseMipLevel = 0;
					barrier.mMipLevels = 1;
					barrier.mBaseDepth = view->mRHIViewDesc.mBaseArrayLayer;
					//理论上应该View使用了Res的某个Layer，只对这个Layer进行Barrier
					//但是RHIResource里只记录了一个State，因此这里对整个Res进行Barrier
					barrier.mDepth = view->mVirtualRes->mDesc.DepthOrArraySize;
					barrier.mBarrierRes = view->mVirtualRes->GetRHIResource();
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
					cmdlist->GetCmdList()->ResourceBarrierExt(barrier);
				}
			}
			

			FGResourceView& dsView = *node->mDS;
			cmdlist->GetCmdList()->BindDescriptorHeap();
			uint32_t width;
			uint32_t height;
			node->mPassDesc.mColorView.clear();
			int index = 0;
			for (FGResourceView* it : node->mRT)
			{
				FGResourceView& rtView = *it;
				width = rtView.mVirtualRes->GetRHIResource()->mResDesc.Width;
				height = rtView.mVirtualRes->GetRHIResource()->mResDesc.Height;
				assert(rtView.mRHIView);
				node->mPassDesc.mColorView.emplace_back(rtView.mRHIView);
			}

			if(node->mPassDesc.mDepths.size() > 0)
			{
				node->mPassDesc.mDepthStencilView = dsView.mRHIView;
				node->mPassDesc.mDepths[0].mDepthStencilFormat = dsView.mRHIView->mBindResource->mResDesc.Format;
			}
			
			
			renderDevice->BeginRenderPass(node->mPassDesc);
			cmdlist->GetCmdList()->SetViewPort(0, 0, width, height);
			cmdlist->GetCmdList()->SetScissorRects(0, 0, width, width);

			node->Execute(this);
			renderDevice->EndRenderPass();

			cmdlist->GetCmdList()->EndEvent();
			cmdlist->GetCmdList()->CloseCommondList();
			renderDevice->mGraphicQueue->ExecuteCommandLists(cmdlist->GetCmdList());
			renderDevice->mGraphicQueue->Signal(mFence3D, ++mFenceValue3D);
		}
	}
	
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
	curIndex = mMaxVirtualResourceId;
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
