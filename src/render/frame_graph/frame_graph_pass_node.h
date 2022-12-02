#pragma once
#include "render/frame_graph/frame_graph_virtual_resource.h"
#include "render/rhi/rhi_render_pass.h"

namespace luna::render
{

class FrameGraphBuilder;
struct FGVirtualResView;

class FGNode 
{
public:
	using PreExecFuncType = std::function<void(FrameGraphBuilder*, FGNode&)>;
	using ExcuteFuncType = std::function<void(FrameGraphBuilder*, FGNode&, RenderDevice*)>;
	using SetupFuncType = std::function<void(FrameGraphBuilder*, FGNode&)>;

	FGNode() = default;
	FGNode(FGNode&) = delete;
	FGNode(const FGNode&) = delete;
	FGNode(FGNode&&) = delete;

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
	
	bool HasResView(const LString& name) { return mVirtureResView.find(name) != mVirtureResView.end(); }

	inline FGVirtualResView& GetResView(const LString& name) { return mVirtureResView[name]; };

	const LMap<LString, FGVirtualResView>& VirtualResViews() { return mVirtureResView; };

	FGNode& AddSRV(FGVirtualTexture* texture);	
	FGNode& AddSRV(FGVirtualBuffer* texture);
	FGNode& AddSRV(const LString& depend, const ViewDesc& desc);

	FGNode& AddRTV(const LString& target, const ViewDesc& desc, RenderPassLoadOp loadOp = RenderPassLoadOp::kDontCare, RenderPassStoreOp storeOp = RenderPassStoreOp::kStore);

	FGNode& AddDSV(const LString& target, const ViewDesc& desc, RenderPassLoadOp loadOp = RenderPassLoadOp::kDontCare, RenderPassStoreOp storeOp = RenderPassStoreOp::kStore);

	FGNode& SetRenderTarget(const LString& rt, const LString& ds)
	{
		mRT = rt;
		mDS = ds;
		return *this;
	}

	const LString& GetName() { return mName; }
	FGNode& Name(const LString& name) { mName = name; return *this; }
	FGNode& ExcuteFunc(ExcuteFuncType func) { mExecFunc = func; return*this; }

private:
	LMap<LString, FGVirtualResView> mVirtureResView;
	LString mRT;
	LString mDS;
	RenderPassDesc mPassDesc;
	ExcuteFuncType mExecFunc;
	PreExecFuncType mPreExecFunc;
	LString mName;

	friend class FrameGraphBuilder;
};
}
