#pragma once
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIPch.h"

namespace luna::graphics
{

class FGGraphDrawNode;
class RENDER_API FrameGraphBuilder
{
public:
	FrameGraphBuilder(const LString& name);

	~FrameGraphBuilder();

	FrameGraphBuilder(const FrameGraphBuilder&) = delete;

	void Clear();

	void CleanUpVirtualMemory();

	FGGraphDrawNode* AddGraphDrawPass(const LString& name);

	LSharedPtr<FGTexture> CreateCommon2DTexture(
		const LString& name,
		uint32_t width,
		uint32_t height,
		RHITextureFormat format,
		RHIImageUsage usage
	);
	
	void Compile();

	void _Prepare();

	void Flush(RHICmdList* cmdList);

	void RemoveVirtualResourceId(size_t virtualResourceId);
private:

	size_t GenerateVirtualResourceId();

	LString mGraphName;
	std::vector<FGNode*> mNodes;

	using RenderPassKey = std::pair<RHIView*, RHIView*>;
	using RenderPassValue = std::pair<RHIRenderPassPtr, RHIFrameBufferPtr>;


	std::unordered_set<size_t> mUnusedVirtualResourceId;
	size_t mMaxVirtualResourceId = 0;

	LUnorderedMap<size_t, RHIResourcePtr> mPhysicResourceMap;

	RHIMemoryPtr				mFGMemory;

	size_t						mFGOffset = 0;

	RHIResourcePtr FGCreateTexture(const RHIResDesc& resDesc);

	RHIResourcePtr FGCreateBuffer(const RHIBufferDesc& resDesc);
};

}
