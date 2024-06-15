#pragma once
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Graphics/RHI/RHIShader.h"
#include "Core/Asset/AssetModule.h"
namespace  luna::graphics
{
void SceneViewTargetData::GenerateViewTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node,LSharedPtr<FGTexture>& colorRt, LSharedPtr<FGTexture>& depthRt, bool clearColor,bool clearDepth)
{
	RenderView* curView = static_cast<RenderView*>(mContainter);
	if (colorRt == nullptr)
	{
		colorRt = builder->CreateCommon2DTexture(
			"ColorResult",
			curView->GetRenderTarget()->GetWidth(),
			curView->GetRenderTarget()->GetHeight(),
			curView->GetRenderTarget()->GetFormat(),
			RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit
		);
		depthRt = builder->CreateCommon2DTexture(
			"DepthResult",
			curView->GetRenderTarget()->GetWidth(),
			curView->GetRenderTarget()->GetHeight(),
			curView->GetRenderTarget()->GetDepthFormat(),
			RHIImageUsage::DepthStencilBit
		);
	}
	FGResourceView* colorView = node->AddRTV(colorRt.get(), RHIViewDimension::TextureView2D);
	FGResourceView* depthView = node->AddDSV(depthRt.get());
	if (clearColor)
	{
		node->SetColorAttachment(colorView, LoadOp::kClear);
	}
	else
	{
		node->SetColorAttachment(colorView, LoadOp::kLoad);
	}
	if (clearDepth)
	{
		node->SetDepthStencilAttachment(depthView, LoadOp::kClear);
	}
	else
	{
		node->SetDepthStencilAttachment(depthView);
	}
}

void SceneViewTargetData::GenerateOpaqueResultRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node, bool clearColor, bool clearDepth)
{
	GenerateViewTarget(builder,node,mOpaqueResultRenderTarget, mOpaqueResultDepthStencil, clearColor, clearDepth);
	RenderView* curView = static_cast<RenderView*>(mContainter);
	RHIResource* colorTexture = curView->GetRenderTarget()->mColorTexture.get();
	RHIResource* depthTexture = curView->GetRenderTarget()->mDepthTexture.get();
	if (!mOpaqueResultRenderTarget->CheckIsExternal())
	{
		mOpaqueResultRenderTarget->BindExternalResource(colorTexture);
		mOpaqueResultDepthStencil->BindExternalResource(depthTexture);
	}
}

void SceneViewTargetData::GeneratePostProcessResultRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node)
{
	GenerateViewTarget(builder, node, mPostProcessResultRenderTarget, mPostProcessResultDepthStencil);
}

void SceneViewTargetData::GenerateScreenRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node)
{
	GenerateViewTarget(builder, node, mScreenRenderTarget, mScreenDepthStencil);
}

void ShadowViewTargetData::GenerateShadowRenderTarget(FrameGraphBuilder* builder, FGGraphDrawNode* node)
{
	RenderView* curView = static_cast<RenderView*>(mContainter);
	if (mShadowRenderTarget == nullptr)
	{
		mShadowRenderTarget = builder->CreateCommon2DTexture(
			"ShadowResult",
			curView->GetRenderTarget()->GetWidth(),
			curView->GetRenderTarget()->GetHeight(),
			curView->GetRenderTarget()->GetFormat(),
			RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit
		);
		mShadowDepthStencil = builder->CreateCommon2DTexture(
			"ShadowDepthResult",
			curView->GetRenderTarget()->GetWidth(),
			curView->GetRenderTarget()->GetHeight(),
			curView->GetRenderTarget()->GetDepthFormat(),
			RHIImageUsage::DepthStencilBit
		);
	}
	FGResourceView* colorView = node->AddRTV(mShadowRenderTarget.get(), RHIViewDimension::TextureView2D);
	FGResourceView* depthView = node->AddDSV(mShadowDepthStencil.get());
	node->SetColorAttachment(colorView, LoadOp::kClear);
	node->SetDepthStencilAttachment(depthView);
}

PARAM_ID(_LUTTex);
PARAM_ID(_EnvTex);
PARAM_ID(_IrradianceTex);
void SceneRenderData::SetMaterialParameter(MaterialInstanceBase* matInstance)
{
	matInstance->SetShaderInput(ParamID__EnvTex, mEnvTex->GetView());
	matInstance->SetShaderInput(ParamID__IrradianceTex, mIrradianceTex->GetView());
	matInstance->SetShaderInput(ParamID__LUTTex, mLUTTex->GetView());
}

SceneRenderData::SceneRenderData()
{
	mEnvTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>(LPath("/assets/built-in/Skybox/Cubemap.dds")));
	mIrradianceTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>(LPath("/assets/built-in/Skybox/IrradianceMap.dds")));
	mLUTTex = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>(LPath("/assets/built-in/Skybox/brdf.dds")));
}


}