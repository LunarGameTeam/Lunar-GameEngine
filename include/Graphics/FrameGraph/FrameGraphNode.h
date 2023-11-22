#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIRenderPass.h"

namespace luna::graphics
{

class FrameGraphBuilder;
struct FGResourceView;

enum class FrameGraphNodeType
{
	FrameGraphNodeGraphDraw,
	FrameGraphNodeCompute
};

class RENDER_API FGAbstractNode
{
	LString mName;
protected:
	LArray<FGResource*> mInputResource;
	LArray<FGResource*> mOutputResource;
public:
	FGAbstractNode() {};
	void SetName(const LString& name) { mName = name; };
	const LString& GetName() { return mName; }

};

class RENDER_API FGNode : public FGAbstractNode
{
public:
	using ExcuteFuncType = std::function<void(FrameGraphBuilder*, FGNode&, RenderContext*)>;

	FGNode(FrameGraphNodeType type);
	FGNode(FGNode&) = delete;
	FGNode(const FGNode&) = delete;
	FGNode(FGNode&&) = delete;
	~FGNode();
	
	FGResourceView* AddSRV(FGTexture*, RHIViewDimension dimension, uint32_t BaseLayer = 0, uint32_t layerCount = 1);
	FGResourceView* AddSRV(FGTexture*, const ViewDesc& desc);

	FGResourceView* AddRTV(FGTexture*, RHIViewDimension dimension, uint32_t BaseLayer = 0, uint32_t layerCount = 1);
	FGResourceView* AddRTV(FGTexture*, const ViewDesc& desc);

	FGResourceView* AddDSV(FGTexture*, const ViewDesc& desc);
	FGResourceView* AddDSV(FGTexture*);

	void Execute(FrameGraphBuilder* builder);

	FGNode& ExcuteFunc(ExcuteFuncType func) { mExecFunc = func; return*this; }

private:
	std::vector<LSharedPtr<FGResourceView>> mVirtureResView;
	
	ExcuteFuncType mExecFunc;

	FrameGraphNodeType mType;
	friend class FrameGraphBuilder;
};

class RENDER_API FGGraphDrawNode : public FGNode
{
public:
	FGGraphDrawNode();

	FGNode& SetDepthClear(const PassDepthStencilDesc& desc);

	FGNode& SetColorAttachment(FGResourceView* rt,
		LoadOp load = LoadOp::kLoad, StoreOp store = StoreOp::kStore,
		const LVector4f& clearColor = LVector4f(0, 0, 0, 1), uint32_t rtIndex = 0);

	FGNode& SetDepthStencilAttachment(FGResourceView* ds, LoadOp load = LoadOp::kLoad, StoreOp store = StoreOp::kStore, float clearDepth = 1.0);
private:
	LArray<FGResourceView*> mRT;

	FGResourceView* mDS;

	RenderPassDesc mPassDesc;
};


}
