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

	FGTexture* CreateTexture(
		uint32_t width, uint32_t height, uint16_t Depth,
		uint16_t miplevels, RHITextureFormat format,
		RHIImageUsage usage,
		const LString& name = "",
		RHIResDimension dimension = RHIResDimension::Texture2D);

	FGTexture* CreateTexture(const RHIResDesc& desc, const LString& name = "");
	
	FGTexture* BindExternalTexture(const RHIResourcePtr& texture, const LString& name = "");

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
