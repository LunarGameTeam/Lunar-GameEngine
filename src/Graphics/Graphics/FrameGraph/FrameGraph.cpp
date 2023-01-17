#include "Graphics/FrameGraph/FrameGraph.h"

#include "Graphics/Renderer/RenderDevice.h"

#include "Graphics/RenderModule.h"
#include "Graphics/RHI/RHIFrameBuffer.h"
#include "Graphics/RHI/RHIFence.h"
#include "Graphics/RHI/RHICmdList.h"
#include "Graphics/RHI/RHIDevice.h"

#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"

#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Asset/ShaderAsset.h"


namespace luna::render
{

FrameGraphBuilder::FrameGraphBuilder(const LString& graph_name)
	:mFenceValue3D(sRenderModule->GetRenderDevice()->mFenceValue)
{
	mGraphName = graph_name;
	mFence3D = sRenderModule->GetRenderDevice()->mFence;
}

FrameGraphBuilder::~FrameGraphBuilder()
{
}

FGNode& FrameGraphBuilder::AddPass(const LString& name)
{
	FGNode* node = new FGNode();
	mNodes.push_back(node);
	return *node;
}


void FrameGraphBuilder::Clear()
{

	//todo::虚拟资源尚未处理隔帧保存的问题，先暂时强制同步
// 	mFenceValue3D = mFence3D.get()->IncSignal(sRenderModule->GetCmdQueueCore());
// 	if (mFenceValue3D != size_t(-1))
// 	{
// 		mFence3D.get()->Wait(mFenceValue3D);
// 	}
	//m_cmd_batch_queue->ResetAllocator();

	for (auto it : mNodes)
	{
		delete it;
	}
	mNodes.clear();


}

FGTexture* FrameGraphBuilder::CreateTexture(
	const LString& res_name,
	const RHIResDesc& res_desc
)
{
	FGTexture* virtualRes = nullptr;
	auto it = mVirtualRes.find(res_name);
	if (it == mVirtualRes.end())
	{
		virtualRes = new FGTexture(res_name, res_desc);
		mVirtualRes[res_name] = virtualRes;
	}
	else
		virtualRes = (FGTexture*)it->second;

	return virtualRes;
}

FGTexture* FrameGraphBuilder::BindExternalTexture(const LString& name, RHIResourcePtr& rhiTexture)
{
	FGTexture* texture = nullptr;
	auto it = mVirtualRes.find(name);
	if (it == mVirtualRes.end())
	{
		texture = new FGTexture(name, rhiTexture);			
		mVirtualRes[name] = texture;		
	}
	else
	{
		texture = static_cast<FGTexture*>(it->second);
		texture->SetRHIResource(rhiTexture);
	}
	return texture;
}

void FrameGraphBuilder::Compile()
{
	//for (auto node_list_member : m_graph_nodes)
	//{
	//	node_list_member->Setup(this);
	//}
}

void FrameGraphBuilder::_Prepare()
{
	for (FGNode* it : mNodes)
	{
		for (FGResourceView* it : it->mVirtureResView)
		{
			RHIViewPtr rhiView = sRenderModule->GetRHIDevice()->CreateView(it->mRHIViewDesc);
			it->mRHIView = rhiView;
			rhiView->BindResource(it->mVirtualRes->mRes);
		}
	}
}

void FrameGraphBuilder::Flush()
{
	RenderDevice* renderDevice = sRenderModule->GetRenderDevice();
	RHIGraphicCmdList* cmdlist = renderDevice->mGraphicCmd;

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


	mFence3D->Wait(mFenceValue3D);
	cmdlist->Reset();
	for (FGNode* node : mNodes)
	{
		for (FGResourceView* view : node->mVirtureResView)
		{			 
			switch (view->mRHIViewDesc.mViewType)
			{
			case RHIViewType::kTexture:
				cmdlist->ResourceBarrierExt({ view->mVirtualRes->GetRHIResource(), ResourceState::kUndefined, ResourceState::kGenericRead });
				break;
			case RHIViewType::kRenderTarget:
				cmdlist->ResourceBarrierExt({ view->mVirtualRes->GetRHIResource(), ResourceState::kUndefined, ResourceState::kRenderTarget });

				break;
			case RHIViewType::kDepthStencil:
				cmdlist->ResourceBarrierExt({ view->mVirtualRes->GetRHIResource(), ResourceState::kUndefined, ResourceState::kDepthStencilWrite });
				break;
			}
		}
	}

	cmdlist->CloseCommondList();
	renderDevice->mGraphicQueue->ExecuteCommandLists(cmdlist);
	renderDevice->mGraphicQueue->Signal(mFence3D, ++mFenceValue3D);


	for (FGNode* node : mNodes)
	{
		if (node->mRT.empty() && !node->mDS)
			return;


		FGResourceView& dsView = *node->mDS;

		mFence3D->Wait(mFenceValue3D);
		cmdlist->Reset();
		cmdlist->BindDescriptorHeap();
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
			node->mPassDesc.mColors[index].mFormat = rtView.mRHIView->mBindResource->mResDesc.Format;
			node->mPassDesc.mColorView.emplace_back(rtView.mRHIView);
		}

		node->mPassDesc.mDepthStencilView = dsView.mRHIView;
		node->mPassDesc.mDepths[0].mDepthStencilFormat = dsView.mRHIView->mBindResource->mResDesc.Format;

		cmdlist->BeginEvent(node->GetName());
		node->PreExecute(this);

		renderDevice->BeginRenderPass(node->mPassDesc);
		cmdlist->SetViewPort(0, 0, width, height);
		cmdlist->SetScissorRects(0, 0, width, width);

		node->Execute(this);
		renderDevice->EndRenderPass();

		cmdlist->EndEvent();

		cmdlist->CloseCommondList();
		renderDevice->mGraphicQueue->ExecuteCommandLists(cmdlist);
		renderDevice->mGraphicQueue->Signal(mFence3D, ++mFenceValue3D);
	}
}


FGTexture* FrameGraphBuilder::GetTexture(const LString& name)
{
	auto it = mVirtualRes.find(name);
	if (it == mVirtualRes.end())
		return nullptr;
	FGTexture* texture = static_cast<FGTexture*>(it->second);
	return texture;
}

}
