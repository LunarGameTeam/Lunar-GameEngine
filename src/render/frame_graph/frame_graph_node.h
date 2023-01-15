#pragma once
#include "core/core_min.h"

#include "render/forward_types.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_render_pass.h"

namespace luna::render
{

class FrameGraphBuilder;
struct FGResourceView;

class RENDER_API FGNode
{
public:
	using PreExecFuncType = std::function<void(FrameGraphBuilder*, FGNode&)>;
	using ExcuteFuncType = std::function<void(FrameGraphBuilder*, FGNode&, RenderDevice*)>;
	using SetupFuncType = std::function<void(FrameGraphBuilder*, FGNode&)>;

	FGNode() = default;
	FGNode(FGNode&) = delete;
	FGNode(const FGNode&) = delete;
	FGNode(FGNode&&) = delete;
	~FGNode();

	void Execute(FrameGraphBuilder* m_builder);;

	void PreExecute(FrameGraphBuilder* m_builder)
	{
		if(mPreExecFunc)
			mPreExecFunc(m_builder, *this);
	};

	FGNode& PreExecFunc(PreExecFuncType func) 
	{
		mPreExecFunc = func;
		return *this;
	}

	FGNode& SetupFunc(FrameGraphBuilder* builder, SetupFuncType func) { func(builder, *this); return*this; }
	
	FGResourceView* AddSRV(FGTexture* texture);

	FGResourceView* AddSRV(FGTexture*, const ViewDesc& desc);
	
	FGResourceView* AddRTV(FGTexture*, const ViewDesc& desc);

	void SetColorClear(const PassColorDesc &desc, uint32_t rtIndex = 0);

	void SetDepthClear(const PassDepthStencilDesc &desc);

	FGResourceView* AddDSV(FGTexture*, const ViewDesc& desc);

	FGNode& SetColorAttachment(FGResourceView* rt, uint32_t rtIndex = 0);
	FGNode& SetDepthStencilAttachment(FGResourceView* ds);

	const LString& GetName() { return mName; }

	FGNode& ExcuteFunc(ExcuteFuncType func) { mExecFunc = func; return*this; }

private:
	std::vector<FGResourceView*> mVirtureResView;
	LArray<FGResourceView*> mRT;
	FGResourceView* mDS;
	RenderPassDesc mPassDesc;
	ExcuteFuncType mExecFunc;
	PreExecFuncType mPreExecFunc;
	LString mName;

	friend class FrameGraphBuilder;
};
}
