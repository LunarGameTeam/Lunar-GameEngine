#include "render/renderer/pass_debug.h"

#include "core/reflection/method.h"
#include "core/asset/asset_module.h"

#include "render/render_module.h"

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

	RHITextureDesc textureDesc;
	textureDesc.heap_flag_in = RHIHeapFlag::HEAP_FLAG_NONE;
	textureDesc.max_size = 0;
	textureDesc.if_force_srgb = false;
	textureDesc.if_gen_mipmap = false;



	RHIResDesc depthResDesc;


	depthResDesc.Desc.mType = ResourceType::kTexture;
	depthResDesc.Desc.Dimension = RHIResDimension::Texture2D;
	depthResDesc.ResHeapType = RHIHeapType::Default;
	depthResDesc.Desc.Width = 1024;
	depthResDesc.Desc.Height = 768;
	depthResDesc.Desc.DepthOrArraySize = 1;
	depthResDesc.Desc.Alignment = 0;
	depthResDesc.Desc.Format = RHITextureFormat::FORMAT_R8G8B8A8_UNORM_SRGB;
	depthResDesc.Desc.mImageUsage = RHIImageUsage::ColorAttachmentBit;
	depthResDesc.Desc.Layout = RHITextureLayout::LayoutUnknown;
	depthResDesc.Desc.MipLevels = 1;
	depthResDesc.Desc.SampleDesc.Count = 1;
	depthResDesc.Desc.SampleDesc.Quality = 0;

	LString rtName = "DebugColor";
	LString rtDepthName = "DebugDepth";

	builder->CreateTexture(rtName, depthResDesc, textureDesc);
	depthResDesc.Desc.Format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	depthResDesc.Desc.mImageUsage = RHIImageUsage::DepthStencilBit;
	builder->CreateTexture(rtDepthName, depthResDesc, textureDesc);

	//用成员函数
	auto& pass = builder->AddPass()
		.Name("DebugPass")
		.SetupFunc(builder, [=, this](FrameGraphBuilder* builder, FGNode& node) {
		ViewDesc desc;
		auto shadow_map = builder->GetTexture(rtName);
		desc.mViewType = RHIViewType::kRenderTarget;
		desc.mViewDimension = RHIViewDimension::TextureView2D;
		node.AddRTV(rtName, desc);

		auto shadow_map_depth = builder->GetTexture(rtDepthName);
		ViewDesc dsvDesc;
		dsvDesc.mViewType = RHIViewType::kDepthStencil;
		dsvDesc.mViewDimension = RHIViewDimension::TextureView2D;
		node.AddDSV(rtDepthName, dsvDesc);

		node.SetRenderTarget(rtName, rtDepthName);


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

		for (auto it : renderScene->GetRenderObjects())
		{
			RenderObject* obj = it;
			renderContext->DrawRenderOBject(obj, debugShader.get(), &params);
		}
		return;

	});

}

}
