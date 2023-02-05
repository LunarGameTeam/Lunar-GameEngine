#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIRenderPass.h"

namespace luna::render
{

class FrameGraphBuilder;
struct FGResourceView;

class RENDER_API FGNode
{
public:
	using PreExecFuncType = std::function<void(FrameGraphBuilder*, FGNode&)>;
	using ExcuteFuncType = std::function<void(FrameGraphBuilder*, FGNode&, RenderContext*)>;
	using SetupFuncType = std::function<void(FrameGraphBuilder*, FGNode&)>;

	FGNode() = default;
	FGNode(FGNode&) = delete;
	FGNode(const FGNode&) = delete;
	FGNode(FGNode&&) = delete;
	~FGNode();

	void Execute(FrameGraphBuilder* builder);

	void PreExecute(FrameGraphBuilder* builder)
	{
		if(mPreExecFunc)
			mPreExecFunc(builder, *this);
	};

	FGNode& PreExecFunc(PreExecFuncType func) 
	{
		mPreExecFunc = func;
		return *this;
	}

	FGNode& SetupFunc(FrameGraphBuilder* builder, SetupFuncType func) { func(builder, *this); return*this; }

	FGResourceView* AddSRV(FGTexture*, RHIViewDimension dimension, uint32_t BaseLayer = 0, uint32_t layerCount = 1);
	FGResourceView* AddSRV(FGTexture*, const ViewDesc& desc);

	FGResourceView* AddRTV(FGTexture*, RHIViewDimension dimension, uint32_t BaseLayer = 0, uint32_t layerCount = 1);
	FGResourceView* AddRTV(FGTexture*, const ViewDesc& desc);

	FGResourceView* AddDSV(FGTexture*, const ViewDesc& desc);
	FGResourceView* AddDSV(FGTexture*);

	void SetDepthClear(const PassDepthStencilDesc &desc);


	FGNode& SetColorAttachment(FGResourceView* rt, LoadOp load = LoadOp::kLoad, StoreOp store = StoreOp::kStore, const LVector4f& clearColor = LVector4f(0,0,0,1), uint32_t rtIndex = 0);
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
