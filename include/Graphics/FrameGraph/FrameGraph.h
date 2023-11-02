#pragma once
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"

namespace luna::graphics
{

class RENDER_API FrameGraphBuilder
{
public:
	FrameGraphBuilder(const LString& name);

	~FrameGraphBuilder();

	FrameGraphBuilder(const FrameGraphBuilder&) = delete;

	void Clear();

	FGNode& AddPass(const LString& name);

	LSharedPtr<FGTexture> CreateCommon2DTexture(
		const LString& name,
		uint32_t width,
		uint32_t height,
		RHITextureFormat format
	);
	
	void Compile();

	void _Prepare();

	void Flush();

	void RemoveVirtualResourceId(size_t virtualResourceId);
private:

	size_t GenerateVirtualResourceId();

	LString mGraphName;
	std::vector<FGNode*> mNodes;

	using RenderPassKey = std::pair<RHIView*, RHIView*>;
	using RenderPassValue = std::pair<RHIRenderPassPtr, RHIFrameBufferPtr>;


	std::unordered_set<size_t> mUnusedVirtualResourceId;
	size_t mMaxVirtualResourceId;


	//frame graph fence
	RHIFencePtr mFence3D;	
	size_t& mFenceValue3D;
};

}
