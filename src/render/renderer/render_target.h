#pragma once

#include "core/object/base_object.h"
#include "core/reflection/reflection.h"

#include "render/render_config.h"
#include "render/asset/texture_asset.h"
#include "render/rhi/rhi_pch.h"


namespace luna::render
{


class RENDER_API RenderTarget : public LObject
{
	RegisterTypeEmbedd(RenderTarget, LObject)
public:
	RenderTarget();

	void Update();
	bool Ready();

	GET_SET_VALUE(RHITextureFormat, mDepthFormat, DepthFormat);

	GET_SET_VALUE(uint32_t, mWidth, Width);
	GET_SET_VALUE(uint32_t, mHeight, Height);

	RHIResource* GetColor() { return mColorTexture.get(); }
	RHIResource* GetDepth() { return mDepthTexture.get(); }

	RHIResourcePtr mColorTexture;
	RHIResourcePtr mDepthTexture;
private:
	uint32_t         mWidth       = 1024;
	uint32_t         mHeight      = 768;
	RHITextureFormat mFormat      = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
	bool             mReady       = false;
	RHITextureFormat mDepthFormat = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
};


}
