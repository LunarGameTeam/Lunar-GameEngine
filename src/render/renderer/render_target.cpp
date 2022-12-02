#include "render_target.h"

#include "render/pch.h"
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
	textureDesc.heap_flag_in = RHIHeapFlag::HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
	textureDesc.if_force_srgb = false;
	textureDesc.if_gen_mipmap = false;
	textureDesc.max_size = 0;
	RHIResDesc resDesc;
	resDesc.ResHeapType = RHIHeapType::Default;
	resDesc.Desc.Dimension = RHIResDimension::Texture2D;
	resDesc.Desc.Width = GetWidth();
	resDesc.Desc.Height = GetHeight();
	resDesc.Desc.DepthOrArraySize = 1;
	resDesc.Desc.Format = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
	resDesc.Desc.Alignment = 0;
	resDesc.Desc.mUsage = RHIResourceUsage::RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resDesc.Desc.Layout = RHITextureLayout::LayoutUnknown;
	resDesc.Desc.MipLevels = 1;
	resDesc.Desc.SampleDesc.Count = 1;
	resDesc.Desc.SampleDesc.Quality = 0;
	resDesc.Desc.mImageUsage = RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit;
	mColorTexture = sRenderModule->mRenderDevice->CreateTexture(textureDesc, resDesc);

	RHIResDesc depthResDesc = resDesc;
	depthResDesc.Desc.Format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	depthResDesc.Desc.SampleDesc.Count = 1;
	depthResDesc.Desc.SampleDesc.Quality = 0;
	depthResDesc.Desc.mImageUsage = RHIImageUsage::DepthStencilBit;
	mDepthTexture = sRenderModule->mRenderDevice->CreateTexture(textureDesc, depthResDesc);
}

bool RenderTarget::Ready()
{
	if (!m_ready)
		Update();
	m_ready = true;
	return m_ready;
}
}