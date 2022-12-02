#include "render/frame_graph/frame_graph_pass_node.h"
#include "render/frame_graph/frame_graph_virtual_resource.h"
#include "render/render_module.h"

namespace luna::render
{

FGNode& FGNode::AddSRV(const LString& depend, const ViewDesc& desc)
{
	FGVirtualResView& view = mVirtureResView[depend];	
	view.mViewDesc = desc;
	return *this;
}


FGNode& FGNode::AddRTV(const LString& target, const ViewDesc& desc, RenderPassLoadOp loadOp, RenderPassStoreOp storeOp)
{
	FGVirtualResView& view = mVirtureResView[target];
	view.mViewDesc = desc;
	return *this;
}

FGNode& FGNode::AddDSV(const LString& target, const ViewDesc& desc, RenderPassLoadOp loadOp, RenderPassStoreOp storeOp)
{
	FGVirtualResView& view = mVirtureResView[target]; 
	view.mViewDesc = desc;
	return *this;
}

FGNode& FGNode::AddSRV(FGVirtualBuffer* texture)
{
	FGVirtualResView& view = mVirtureResView[texture->GetName()];
	return *this;

}

FGNode& FGNode::AddSRV(FGVirtualTexture* texture)
{
	return *this;
}

void FGNode::Execute(FrameGraphBuilder* m_builder)
{
	if(mExecFunc)
		mExecFunc(m_builder, *this, sRenderModule->GetRenderDevice());
}

}
