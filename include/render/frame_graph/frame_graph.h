#pragma once
#include "render/forward_types.h"
#include "render/rhi/rhi_types.h"

namespace luna::render
{

class RENDER_API FrameGraphBuilder
{
public:
	FrameGraphBuilder(const LString& name);

	~FrameGraphBuilder();

	FrameGraphBuilder(const FrameGraphBuilder&) = delete;

	void Clear();

	FGNode& AddPass(const LString& name);

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
	LArray<RHIViewPtr> mConstantBuffer;

	//frame graph fence
	RHIFencePtr mFence3D;	
	size_t& mFenceValue3D;
};

}
