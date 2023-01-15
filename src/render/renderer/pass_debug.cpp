#include "render/renderer/pass_debug.h"

#include "core/reflection/method.h"
#include "core/asset/asset_module.h"

#include "render/asset/shader_asset.h"
#include "render/render_module.h"
#include "render/renderer/render_view.h"
#include "render/renderer/material.h"
#include "render/frame_graph/frame_graph_resource.h"

namespace luna::render
{

DebugRenderPass::DebugRenderPass()
{


}

void DebugRenderPass::InitImpl()
{

}
void DebugRenderPass::BuildRenderPass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{

	static LSharedPtr<ShaderAsset> debugShader = LSharedPtr<ShaderAsset>(sAssetModule->LoadAsset<render::ShaderAsset>("/assets/built-in/Debug.hlsl"));

	static std::pair<RHIPipelineStatePtr, RHIBindingSetPtr> pipeline;
	//用成员函数
	auto& pass = builder->AddPass("DebugPass");
	pass.SetupFunc(builder, [=, this](FrameGraphBuilder* builder, FGNode& node) 
	{
		LString rtName = "MainColor";
		LString rtDepthName = "MainDepth";
		RHIResourcePtr colorTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mColorTexture : sRenderModule->mMainRT->mColorTexture;
		RHIResourcePtr depthTexture = view->GetRenderTarget() ? view->GetRenderTarget()->mDepthTexture : sRenderModule->mMainRT->mDepthTexture;
		builder->BindExternalTexture(rtName, colorTexture);
		builder->BindExternalTexture(rtDepthName, depthTexture);

		//builder->BindExternalTexture(mShadowMapDsName, view->GetRenderTarget()->mDepthTexture);

		FGTexture* color = builder->GetTexture(rtName);
		FGTexture* depth = builder->GetTexture(rtDepthName);

		assert(color);
		assert(depth);

		ViewDesc srvDesc;
		srvDesc.mViewType = RHIViewType::kTexture;
		srvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		ViewDesc rtvDesc;
		rtvDesc.mViewType = RHIViewType::kRenderTarget;
		rtvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		ViewDesc dsvDesc;
		dsvDesc.mViewType = RHIViewType::kDepthStencil;
		dsvDesc.mViewDimension = RHIViewDimension::TextureView2D;

		auto colorView = node.AddRTV(color, rtvDesc);
		auto depthView = node.AddDSV(depth, dsvDesc);
		node.SetColorAttachment(colorView);
		node.SetDepthStencilAttachment(depthView);
	});

	static PackedParams params;

	pass.PreExecFunc([=, this](FrameGraphBuilder* builder, FGNode& node) {
		params.Clear();
		PARAM_ID(ViewBuffer);
		PARAM_ID(ObjectBuffer);
		PARAM_ID(SceneBuffer);
		params.PushShaderParam(ParamID_ViewBuffer, view->GetPerViewBufferView());
		params.PushShaderParam(ParamID_ObjectBuffer, renderScene->mROBufferView);
		params.PushShaderParam(ParamID_SceneBuffer, renderScene->mSceneBufferView);				
	});

	pass.ExcuteFunc([=, this](FrameGraphBuilder* builder, FGNode& node, RenderDevice* renderContext) {

		//renderContext->DrawMesh(&renderScene->mDebugMesh, debugShader.get(), &params);		
		return;

	});

}

}
