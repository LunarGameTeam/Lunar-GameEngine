#include "Graphics/Renderer/RenderTarget.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderModule.h"
#include <vector>


namespace luna::graphics
{

RegisterTypeEmbedd_Imp(RenderTarget)
{
	cls->Ctor<RenderTarget>();
	cls->Binding<RenderTarget>();
	cls->BindingProperty<&RenderTarget::mWidth>("width");

	cls->VirtualProperty("color_texture")
		.Getter<&RenderTarget::GetColor>()
		.Binding<RenderTarget, RHIResource*>();

	cls->BindingProperty<&RenderTarget::mHeight>("height");
	cls->BindingMethod<&RenderTarget::Update>("update");
	BindingModule::Get("luna")->AddType(cls);
};

RHIResource* RenderTarget::GetColor()
{ 
	return mColorTexture.get();
}

RHIResource* RenderTarget::GetDepth()
{ 
	return mDepthTexture.get();
}

RenderTarget::RenderTarget()
{

}

void RenderTarget::Update()
{
	RHIResDesc resDesc;
	resDesc.mType = ResourceType::kTexture;
	resDesc.Dimension = RHIResDimension::Texture2D;
	resDesc.Width = GetWidth();
	resDesc.Height = GetHeight();
	resDesc.DepthOrArraySize = 1;
	resDesc.Format = RHITextureFormat::R8G8B8A8_UNORM;
	resDesc.Alignment = 0;
	resDesc.mUsage = RHIResourceUsage::RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resDesc.Layout = RHITextureLayout::LayoutUnknown;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.mImageUsage = RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit;
	mColorTexture = sRenderModule->mRenderContext->CreateTexture(resDesc);

	RHIResDesc depthResDesc = resDesc;
	depthResDesc.Format = RHITextureFormat::D24_UNORM_S8_UINT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.SampleDesc.Quality = 0;
	depthResDesc.mImageUsage = RHIImageUsage::DepthStencilBit;
	mDepthTexture = sRenderModule->mRenderContext->CreateTexture(depthResDesc);

	OnRenderTargetDirty.BroadCast(this);
}

bool RenderTarget::Ready()
{
	if (!mReady)
		Update();
	mReady = true;
	return mReady;
}
}