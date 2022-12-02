#pragma once
#include "render/frame_graph/frame_graph_pass_node.h"
#include "render/renderer/render_device.h"
#include "frame_graph_virtual_resource.h"
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

	FGNode& AddPass()
	{
		FGNode* node = new FGNode();
		mNodes.push_back(node);
		return *node;
	};

	FGVirtualTexture* CreateTexture(const LString& name, const RHIResDesc& desc, const RHITextureDesc& textureDesc);

	FGVirtualBuffer* BindExternalBuffer(const LString& name, RHIResourcePtr& buffer);

	FGVirtualTexture* BindExternalTexture(const LString& name, RHIResourcePtr& texture);	

	FGVirtualTexture* GetTexture(const LString& name);
	
	void Compile();

	void _Prepare();

	void Flush();

private:

	LString mGraphName;
	std::vector<FGNode*> mNodes;

	using RenderPassKey = std::pair<RHIView*, RHIView*>;
	using RenderPassValue = std::pair<RHIRenderPassPtr, RHIFrameBufferPtr>;

	RenderPassValue GetOrCreateRenderPass(FGNode* node);

	std::map<RenderPassKey, RenderPassValue> mRHIFrameBuffers;

	std::map<LString, FGVirtualRes*> mVirtualRes;	
	LVector<RHIViewPtr> mConstantBuffer;

	//frame graph fence
	RHIFencePtr mFence3D;	
	size_t& mFenceValue3D;
};

}
