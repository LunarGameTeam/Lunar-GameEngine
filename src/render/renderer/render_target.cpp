#include "render_target.h"

#include "render/render_config.h"
#include "render/render_module.h"
#include <vector>


namespace luna::render
{

RenderTarget::RenderTarget()
{

}

void RenderTarget::Update()
{
	RHITextureDesc textureDesc;
	textureDesc.if_force_srgb = false;
	textureDesc.if_gen_mipmap = false;
	textureDesc.max_size = 0;
	RHIResDesc resDesc;
	resDesc.mType = ResourceType::kTexture;
	resDesc.Dimension = RHIResDimension::Texture2D;
	resDesc.Width = GetWidth();
	resDesc.Height = GetHeight();
	resDesc.DepthOrArraySize = 1;
	resDesc.Format = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
	resDesc.Alignment = 0;
	resDesc.mUsage = RHIResourceUsage::RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resDesc.Layout = RHITextureLayout::LayoutUnknown;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.mImageUsage = RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit;
	mColorTexture = sRenderModule->mRenderDevice->CreateTexture(textureDesc, resDesc);

	RHIResDesc depthResDesc = resDesc;
	depthResDesc.Format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.SampleDesc.Quality = 0;
	depthResDesc.mImageUsage = RHIImageUsage::DepthStencilBit;
	mDepthTexture = sRenderModule->mRenderDevice->CreateTexture(textureDesc, depthResDesc);

}

bool RenderTarget::Ready()
{
	if (!mReady)
		Update();
	mReady = true;
	return mReady;
}
}