#pragma once
#include "render/frame_graph/frame_graph_node.h"
#include "render/renderer/render_device.h"
#include "frame_graph_resource.h"
#include "render/asset/texture_asset.h"
#include "render/asset/shader_asset.h"


namespace luna::render
{

class FrameGraphBuilder
{
public:
	FrameGraphBuilder(const LString& name);

	~FrameGraphBuilder();

	FrameGraphBuilder(const FrameGraphBuilder&) = delete;

	void Clear();

	FGNode& AddPass(const LString& name)
	{
		FGNode* node = new FGNode();
		mNodes.push_back(node);
		return *node;
	};

	FGTexture* CreateTexture(const LString& name, const RHIResDesc& desc);
	
	FGTexture* BindExternalTexture(const LString& name, RHIResourcePtr& texture);	

	FGTexture* GetTexture(const LString& name);
	
	void Compile();

	void _Prepare();

	void Flush();

private:

	LString mGraphName;
	std::vector<FGNode*> mNodes;

	using RenderPassKey = std::pair<RHIView*, RHIView*>;
	using RenderPassValue = std::pair<RHIRenderPassPtr, RHIFrameBufferPtr>;

	std::map<RenderPassKey, RenderPassValue> mRHIFrameBuffers;

	std::map<LString, FGResource*> mVirtualRes;	
	LVector<RHIViewPtr> mConstantBuffer;

	//frame graph fence
	RHIFencePtr mFence3D;	
	size_t& mFenceValue3D;
};

}
