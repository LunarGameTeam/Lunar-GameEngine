#include "render/frame_graph/frame_graph.h"
#include "render/render_module.h"
#include "render/rhi/rhi_frame_buffer.h"
#include "utility"


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

void FrameGraphBuilder::Clear()
{

	//todo::虚拟资源尚未处理隔帧保存的问题，先暂时强制同步
// 	mFenceValue3D = mFence3D.get()->IncSignal(sRenderModule->GetCmdQueueCore());
// 	if (mFenceValue3D != size_t(-1))
// 	{
// 		mFence3D.get()->Wait(mFenceValue3D);
// 	}
	//m_cmd_batch_queue->ResetAllocator();

	for (auto frame_graph_node : mNodes)
	{
		delete frame_graph_node;
	}
	mNodes.clear();
// 	for (auto it : mVirtualRes)
// 	{
// 		delete it.second;
// 	}
// 	mVirtualRes.clear();


}

FGVirtualBuffer* FrameGraphBuilder::BindExternalBuffer(
	const LString& name,
	RHIResourcePtr& buffer
)
{
	FGVirtualBuffer* virtualRes = nullptr;
	auto it = mVirtualRes.find(name);
	if (it == mVirtualRes.end())
	{
		RHIResDesc new_desc;
		virtualRes = new FGVirtualBuffer(name, new_desc);
		virtualRes->SetRHIResource(buffer);
		mVirtualRes[name] = virtualRes;
	}
	return virtualRes;
}

FGVirtualTexture* FrameGraphBuilder::CreateTexture(
	const LString& res_name,
	const RHIResDesc& res_desc,
	const RHITextureDesc& texture_desc
)
{
	FGVirtualTexture* virtualRes = nullptr;
	auto it = mVirtualRes.find(res_name);
	if (it == mVirtualRes.end())
	{
		virtualRes = new FGVirtualTexture(res_name, res_desc, texture_desc);
		mVirtualRes[res_name] = virtualRes;
	}
	else
		virtualRes = (FGVirtualTexture*)it->second;

	return virtualRes;
}

FGVirtualTexture* FrameGraphBuilder::BindExternalTexture(
	const LString& name,
	RHIResourcePtr& rhiTexture
)
{
	FGVirtualTexture* texture = nullptr;
	auto it = mVirtualRes.find(name);
	if (it == mVirtualRes.end())
	{
		RHITextureDesc desc;
		texture = new FGVirtualTexture(name, rhiTexture->GetDesc(), desc);
		texture->SetRHIResource(rhiTexture);		
		mVirtualRes[name] = texture;		
	}
	else
	{
		texture = dynamic_cast<FGVirtualTexture*>(it->second);
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
		for (auto& it : it->mVirtureResView)
		{
			const LString& name = it.first;
			FGVirtualResView& view = it.second;

			RHIViewPtr rhiView;

			auto resIt = mVirtualRes.find(name);
			if (resIt == mVirtualRes.end())
				continue;
			view.VirtualRes = resIt->second;
			rhiView = sRenderModule->GetRenderDevice()->CreateView(view.mViewDesc);			
			view.mView = rhiView;
			rhiView->BindResource(view.VirtualRes->GetRHIResource());

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
			if (it.second->GetDesc().mType == ResourceType::kBuffer)
			{
				FGVirtualBuffer* res_buf = dynamic_cast<FGVirtualBuffer*>(it.second);				
			}
			else if (it.second->GetDesc().mType == ResourceType::kTexture)
			{
				FGVirtualTexture* virtualRes = dynamic_cast<FGVirtualTexture*>(it.second);
				RHIResourcePtr rhiRes = renderDevice->CreateFGTexture(virtualRes->GetTextureDesc(), virtualRes->GetDesc());
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
		for (auto& it : node->mVirtureResView)
		{
			const LString& name = it.first;
			FGVirtualResView& view = it.second;
			switch (view.mViewDesc.mViewType)
			{
			case RHIViewType::kTexture:
				cmdlist->ResourceBarrierExt({ view.VirtualRes->GetRHIResource(), ResourceState::kUndefined, ResourceState::kGenericRead });
				break;
			case RHIViewType::kRenderTarget:
				cmdlist->ResourceBarrierExt({ view.VirtualRes->GetRHIResource(), ResourceState::kUndefined, ResourceState::kRenderTarget });

				break;
			case RHIViewType::kDepthStencil:
				cmdlist->ResourceBarrierExt({ view.VirtualRes->GetRHIResource(), ResourceState::kUndefined, ResourceState::kDepthStencilWrite });
				break;
			}
		}
	}
	cmdlist->CloseCommondList();
	renderDevice->mGraphicQueue->ExecuteCommandLists(cmdlist);
	renderDevice->mGraphicQueue->Signal(mFence3D, ++mFenceValue3D);


	for (FGNode* node : mNodes)
	{
		if (!node->HasResView(node->mRT) && !node->HasResView(node->mDS))
			return;

		FGVirtualResView& rtView = node->mVirtureResView[node->mRT];
		FGVirtualResView& dsView = node->mVirtureResView[node->mDS];

		auto width = rtView.VirtualRes->GetRHIResource()->GetDesc().Width;
		auto height = rtView.VirtualRes->GetRHIResource()->GetDesc().Height;

		auto it = GetOrCreateRenderPass(node);


		mFence3D->Wait(mFenceValue3D);
		cmdlist->Reset();
		cmdlist->BindDescriptorHeap();

		node->PreExecute(this);
		cmdlist->BeginEvent(node->GetName());

		//
		renderDevice->BeginRenderPass(it.first, it.second);
		cmdlist->SetViewPort(0, 0, width, height);
		cmdlist->SetScissorRects(0, 0, width, width);
		//cmdlist->ClearRTView(rtView.mView, LVector4f(0, 0, 0, 1));
		//cmdlist->ClearDSView(0, 0, width, height, dsView.mView, 1, 0);

		node->Execute(this);
		renderDevice->EndRenderPass();

		cmdlist->EndEvent();

		cmdlist->CloseCommondList();
		renderDevice->mGraphicQueue->ExecuteCommandLists(cmdlist);
		renderDevice->mGraphicQueue->Signal(mFence3D, ++mFenceValue3D);
	}
}


FGVirtualTexture* FrameGraphBuilder::GetTexture(const LString& name)
{
	auto it = mVirtualRes.find(name);
	if (it == mVirtualRes.end())
		return nullptr;
	FGVirtualTexture* texture = dynamic_cast<FGVirtualTexture*>(it->second);
	return texture;
}

FrameGraphBuilder::RenderPassValue FrameGraphBuilder::GetOrCreateRenderPass(FGNode* node)
{

	FGVirtualResView& rtView = node->mVirtureResView[node->mRT];
	FGVirtualResView& dsView = node->mVirtureResView[node->mDS];


	auto width = rtView.VirtualRes->GetRHIResource()->GetDesc().Width;
	auto height = rtView.VirtualRes->GetRHIResource()->GetDesc().Height;

	auto key = std::make_pair(rtView.mView, dsView.mView);
	if (mRHIFrameBuffers.find(key) != mRHIFrameBuffers.end())
		return mRHIFrameBuffers[key];

	RenderPassDesc passDesc;

	RenderPassColorDesc colorDesc;	
	colorDesc.mFormat = rtView.VirtualRes->GetRHIResource()->GetDesc().Format;
	colorDesc.mClearColor = LVector4f(0.2, 0.2, 0.2, 1);
	passDesc.mColors.push_back(colorDesc);

	RenderPassDepthStencilDesc depthDesc;
	depthDesc.mDepthStencilFormat = dsView.VirtualRes->GetRHIResource()->GetDesc().Format;
	depthDesc.mClearDepth = 1.0f;
	depthDesc.mClearStencil = 0;
	passDesc.mDepths.push_back(depthDesc);

	RHIRenderPassPtr pass = sRenderModule->GetRHIDevice()->CreateRenderPass(passDesc);

	FrameBufferDesc desc;
	desc.mPass = pass;
	desc.mColor.push_back(rtView.mView);
	desc.mDepthStencil = dsView.mView;

	desc.mWidth = width;
	desc.mHeight = height;
	RHIFrameBufferPtr frameBuffer = sRenderModule->GetRHIDevice()->CreateFrameBuffer(desc);
	mRHIFrameBuffers[key] = std::make_pair(pass, frameBuffer);
	return mRHIFrameBuffers[key];
}
}
