#include "render/frame_graph/frame_graph_node.h"
#include "render/frame_graph/frame_graph_resource.h"
#include "render/render_module.h"

namespace luna::render
{

FGResourceView* FGNode::AddSRV(FGTexture* res, const ViewDesc& desc)
{
	mVirtureResView.emplace_back(new FGResourceView);
	FGResourceView* view = mVirtureResView.back();
	view->mRHIViewDesc = desc;
	view->mVirtualRes = res;
	return view;
}

FGNode::~FGNode()
{
	for (auto it : mVirtureResView)
	{
		delete it;
	}

}


FGResourceView* FGNode::AddRTV(FGTexture* res, const ViewDesc& desc)
{
	mVirtureResView.emplace_back(new FGResourceView);
	FGResourceView* view = mVirtureResView.back();
	view->mRHIViewDesc = desc;
	view->mVirtualRes = res;
	return view;
}

FGResourceView* FGNode::AddDSV(FGTexture* res, const ViewDesc& desc)
{
	mVirtureResView.emplace_back(new FGResourceView);
	FGResourceView* view = mVirtureResView.back();
	view->mRHIViewDesc = desc;
	view->mVirtualRes = res;
	return view;
}

FGResourceView* FGNode::AddSRV(FGTexture* texture)
{
	return nullptr;
}

void FGNode::Execute(FrameGraphBuilder* m_builder)
{
	if(mExecFunc)
		mExecFunc(m_builder, *this, sRenderModule->GetRenderDevice());
}

void FGNode::SetColorClear(const PassColorDesc& desc, uint32_t rtIndex)
{
	mPassDesc.mColors.resize(rtIndex + 1);
	mPassDesc.mColors[rtIndex] = desc;
}

void FGNode::SetDepthClear(const PassDepthStencilDesc& desc)
{
	mPassDesc.mDepths.emplace_back(desc);
}

FGNode& FGNode::SetColorAttachment(FGResourceView* rt, uint32_t rtIndex /*= 0*/)
{
	mPassDesc.mColors.resize(rtIndex + 1);	
	mRT.resize(rtIndex + 1);
	mRT[rtIndex] = rt;
	return *this;
}

FGNode& FGNode::SetDepthStencilAttachment(FGResourceView* ds)
{
	mPassDesc.mDepths.resize(1);	
	mPassDesc.mDepthStencilView = ds->mRHIView;
	mDS = ds;
	return *this;
}

}
