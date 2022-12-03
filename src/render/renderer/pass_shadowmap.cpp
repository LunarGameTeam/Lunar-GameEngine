#include "render/renderer/pass_shadowmap.h"

#include "core/reflection/method.h"
#include "core/asset/asset_module.h"
#include "render/render_module.h"

namespace luna::render
{

void ShadowRenderPass::InitImpl()
{
	mShadowShader = LSharedPtr<ShaderAsset>(sAssetModule->LoadAsset<render::ShaderAsset>("/assets/built-in/depth.hlsl"));

	textureDesc.heap_flag_in = RHIHeapFlag::HEAP_FLAG_NONE;
	textureDesc.max_size = 0;
	textureDesc.if_force_srgb = false;
	textureDesc.if_gen_mipmap = false;


	shadowTextureDesc.Dimension = RHIResDimension::Texture2D;
	shadowTextureDesc.mType = ResourceType::kTexture;
	shadowTextureDesc.ResHeapType = RHIHeapType::Default;
	shadowTextureDesc.Width = width;
	shadowTextureDesc.Height = height;
	shadowTextureDesc.DepthOrArraySize = 1;
	shadowTextureDesc.Format = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
	shadowTextureDesc.Alignment = 0;
	shadowTextureDesc.mImageUsage = RHIImageUsage::ColorAttachmentBit;
	shadowTextureDesc.Layout = RHITextureLayout::LayoutUnknown;
	shadowTextureDesc.MipLevels = 1;
	shadowTextureDesc.SampleDesc.Count = 1;
	shadowTextureDesc.SampleDesc.Quality = 0;

	shadowDepthTextureDesc = shadowTextureDesc;
	shadowDepthTextureDesc.mType = ResourceType::kTexture;
	shadowDepthTextureDesc.Format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	shadowDepthTextureDesc.SampleDesc.Count = 1;
	shadowDepthTextureDesc.SampleDesc.Quality = 0;
	shadowDepthTextureDesc.mImageUsage = RHIImageUsage::DepthStencilBit;

}

void ShadowRenderPass::RenderPassSetup(FrameGraphBuilder* builder, FGNode& node)
{

}

void ShadowRenderPass::RenderPassExcute(FrameGraphBuilder* builder, FGNode& node, RenderDevice* device)
{

	/*
	RHIConstantBufferDesc& matrixBufferDesc = pipeline->GetConstantBufferDesc("MatrixBuffer");
	FGBufferUniform* matrix_buffer = builder->CreateUniformBuffer(matrixBufferDesc);

	RHIConstantBufferDesc& passBufferDesc = pipeline->GetConstantBufferDesc("PassBuffer");
	FGBufferUniform* pass_buffer = builder->CreateUniformBuffer(passBufferDesc);
	ConstantBufferVar& matrix_var = matrixBufferDesc.mVars["worldMatrix"];

	{
		ConstantBufferVar& var = passBufferDesc.mVars["_viewMatrix"];
		pass_buffer->mBuffer->GetRHIResource()->UpdateUploadBuffer(pass_buffer->mOffset + var.mOffset, &mLight->GetViewMatrix(), var.mSize);
	}
	{
		ConstantBufferVar& var = passBufferDesc.mVars["lightProjectionMatrix"];
		pass_buffer->mBuffer->GetRHIResource()->UpdateUploadBuffer(pass_buffer->mOffset + var.mOffset, &mLight->GetProjectionMatrix(), var.mSize);
	}
	*/
}

void ShadowRenderPass::BuildRenderPass(
	FrameGraphBuilder* builder,
	RenderView* view_info,
	RenderScene* render_scene)
{
	RenderPass::BuildRenderPass(builder, view_info, render_scene);

	mLight = &render_scene->GetMainDirLight();
	std::string name_tail = "Scene" + std::to_string(render_scene->GetSceneId()) + "_View" + std::to_string(view_info->GetViewID());
	mShadowMapName = "ShadowMap_" + name_tail;
	mShadowMapDepthName = "ShadowMapDepth_" + name_tail;
	builder->BindExternalTexture(mShadowMapName, sRenderModule->mMainRT->mColorTexture);
	//builder->CreateTexture(mShadowMapName, shadowTextureDesc, textureDesc);
	builder->CreateTexture(
		mShadowMapDepthName, shadowDepthTextureDesc, textureDesc);

	//用成员函数
	auto& pass = builder->AddPass()
		.Name("ShadowPass")
		.SetupFunc(builder, [this](FrameGraphBuilder* builder, FGNode& node) {
		ViewDesc desc;
		auto shadow_map = builder->GetTexture(mShadowMapName);
		desc.mViewType = RHIViewType::kRenderTarget;		
		node.AddRTV(mShadowMapName, desc);

		auto shadow_map_depth = builder->GetTexture(mShadowMapDepthName);
		ViewDesc dsvDesc;
		dsvDesc.mViewType = RHIViewType::kDepthStencil;
		dsvDesc.mViewDimension = RHIViewDimension::TextureView2D;
		node.AddDSV(mShadowMapDepthName, dsvDesc);

		node.SetRenderTarget(mShadowMapName, mShadowMapDepthName);

		});

	pass.PreExecFunc([this](FrameGraphBuilder* builder, FGNode& node) {

// 		std::vector<BindingDesc> bindings;
// 
// 		if (mShadowPipeline.first == nullptr)
// 			mShadowPipeline = builder->GetPipelineState(&node, mShadowShader.get());
// 
// 		bindings.emplace_back(mShadowShader->GetBindPoint("PassBuffer"), mPassView);
// 
// 		bindings.emplace_back(mShadowShader->GetBindPoint("ObjectBuffer"), mRenderScene->mROBufferView);
// 		mShadowPipeline.second->WriteBindings(bindings);
		});
	pass.ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RenderDevice* renderContext) {

		auto renderDevice = sRenderModule->GetRenderDevice();

		sRenderModule->GetRenderDevice()->mGraphicCmd->SetPipelineState(mShadowPipeline.first);
		std::vector<BindingDesc> bindings;

		renderDevice->mGraphicCmd->BindDesriptorSetExt(mShadowPipeline.second);

		for (auto it : mRenderScene->GetRenderObjects())
		{
			RenderObject* obj = it;
			renderDevice->DrawRenderOBject(obj, nullptr, nullptr);
		}
		return;

		});

}

}
