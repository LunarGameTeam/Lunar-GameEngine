#pragma once

#include "Core/Object/BaseObject.h"
#include "Core/Reflection/Reflection.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/RHI/RHIPch.h"


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
	RHITextureFormat mFormat      = RHITextureFormat::R8G8B8A8_UNORM;
	bool             mReady       = false;
	RHITextureFormat mDepthFormat = RHITextureFormat::D24_UNORM_S8_UINT;
};


}
