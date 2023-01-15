#include "render/renderer/pass_shadowmap.h"

#include "core/reflection/method.h"
#include "core/asset/asset_module.h"
#include "render/render_module.h"
#include "render/asset/shader_asset.h"

namespace luna::render
{

void ShadowRenderPass::InitImpl()
{
	mShadowShader = LSharedPtr<ShaderAsset>(sAssetModule->LoadAsset<render::ShaderAsset>("/assets/built-in/depth.hlsl"));

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

	
	LString shadowMapName = "ShadowMap";
	LString shadowMapDepthName = "ShadowMapDepth";

	FGTexture* shadowmap = builder->BindExternalTexture(shadowMapName, sRenderModule->mMainRT->mColorTexture);
	//builder->CreateTexture(mShadowMapName, shadowTextureDesc, textureDesc);
	FGTexture* shadowmapDepth = builder->CreateTexture(
		shadowMapDepthName, shadowDepthTextureDesc);

	//用成员函数
	auto& pass = builder->AddPass("ShadowPass");
	pass.SetupFunc(builder, [=, this](FrameGraphBuilder* builder, FGNode& node) {
		ViewDesc desc;
	desc.mViewType = RHIViewType::kRenderTarget;
	auto rtv = node.AddRTV(shadowmap, desc);

	ViewDesc dsvDesc;
	dsvDesc.mViewType = RHIViewType::kDepthStencil;
	dsvDesc.mViewDimension = RHIViewDimension::TextureView2D;
	auto dsv = node.AddDSV(shadowmapDepth, dsvDesc);

	node.SetColorAttachment(rtv);
	node.SetDepthStencilAttachment(dsv);

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
	pass.ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RenderDevice* renderContext) 
		{

		auto renderDevice = sRenderModule->GetRenderDevice();

	for (auto it : mRenderScene->GetRenderObjects())
	{
		RenderObject* obj = it;
		renderDevice->DrawRenderOBject(obj, nullptr, nullptr);
	}
	return;

		});

}

}
