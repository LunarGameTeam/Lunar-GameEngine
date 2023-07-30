#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/RenderModule.h"

namespace luna::graphics
{

FGResourceView* FGNode::AddSRV(FGTexture* res, const ViewDesc& desc)
{
	mVirtureResView.emplace_back(new FGResourceView);
	FGResourceView* view = mVirtureResView.back();
	view->mRHIViewDesc = desc;
	view->mVirtualRes = res;
	return view;
}

FGResourceView* FGNode::AddSRV(FGTexture* res, RHIViewDimension dimension, uint32_t BaseLayer /*= 0*/, uint32_t layerCount /*= 1*/)
{
	mVirtureResView.emplace_back(new FGResourceView);
	FGResourceView* view = mVirtureResView.back();
	view->mRHIViewDesc.mViewType = RHIViewType::kTexture;
	view->mRHIViewDesc.mViewDimension = dimension;
	view->mRHIViewDesc.mBaseArrayLayer = BaseLayer;
	view->mRHIViewDesc.mLayerCount = layerCount;
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

FGResourceView* FGNode::AddRTV(FGTexture* res, RHIViewDimension dimension, uint32_t BaseLayer /*= 0*/, uint32_t layerCount /*= 1*/)
{
	mVirtureResView.emplace_back(new FGResourceView);
	FGResourceView* view = mVirtureResView.back();
	view->mRHIViewDesc.mViewType = RHIViewType::kRenderTarget;
	view->mRHIViewDesc.mViewDimension = dimension;
	view->mRHIViewDesc.mBaseArrayLayer = BaseLayer;
	view->mRHIViewDesc.mLayerCount = layerCount;
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

FGResourceView* FGNode::AddDSV(FGTexture* res)
{
	mVirtureResView.emplace_back(new FGResourceView);
	FGResourceView* view = mVirtureResView.back();
	view->mRHIViewDesc.mViewType = RHIViewType::kDepthStencil;
	view->mRHIViewDesc.mViewDimension = RHIViewDimension::TextureView2D;
	view->mVirtualRes = res;
	return view;
}

void FGNode::Execute(FrameGraphBuilder* m_builder)
{
	ZoneScoped;
	if(mExecFunc)
		mExecFunc(m_builder, *this, sRenderModule->GetRenderContext());
}

FGNode& FGNode::SetDepthClear(const PassDepthStencilDesc& desc)
{
	mPassDesc.mDepths.emplace_back(desc);
	return *this;
}


FGNode& FGNode::SetColorAttachment(FGResourceView* rt, LoadOp load /*= LoadOp::kLoad*/, StoreOp store /*= StoreOp::kStore*/, const LVector4f& clearColor /*= LVector4f(0,0,0,1)*/, uint32_t rtIndex /*= 0*/)
{
	if(rtIndex >= mPassDesc.mColors.size())
		mPassDesc.mColors.resize(rtIndex + 1);
	mPassDesc.mColors[rtIndex].mLoadOp = load;
	mPassDesc.mColors[rtIndex].mStoreOp = store;
	mPassDesc.mColors[rtIndex].mClearColor = clearColor;

	if(rtIndex >= mRT.size())
		mRT.resize(rtIndex + 1);

	mRT[rtIndex] = rt;
	return *this;
}

FGNode& FGNode::SetDepthStencilAttachment(FGResourceView* ds, LoadOp load /*= LoadOp::kLoad*/, StoreOp store /*= StoreOp::kStore*/, float clearDepth /*= LVector4f(0,0,0,1)*/)
{
	mPassDesc.mDepths.resize(1);	
	mPassDesc.mDepthStencilView = ds->mRHIView;
	mPassDesc.mDepths[0].mDepthLoadOp = load;
	mPassDesc.mDepths[0].mDepthStoreOp = store;
	mPassDesc.mDepths[0].mClearDepth = clearDepth;
	mDS = ds;
	return *this;
}

}
