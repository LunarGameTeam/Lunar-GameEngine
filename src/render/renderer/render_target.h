#pragma once

#include "core/object/base_object.h"
#include "render/pch.h"
#include "render/asset/texture_asset.h"
#include "render/rhi/rhi_pch.h"


namespace luna::render
{
class RENDER_API RenderTarget : public LObject
{
public:
	RenderTarget();
	void Update();
	bool Ready();

	bool GetScreenRt()
	{
		return m_screen_rt;
	}

	void SetScreenRt(bool val)
	{
		m_screen_rt = val;
	}

	void SetWindowHandle()
	{

	}

	GET_SET_VALUE(RHITextureFormat, m_depth_format, DepthFormat);

	GET_SET_VALUE(uint32_t, mWidth, Width);
	GET_SET_VALUE(uint32_t, mHeight, Height);

	RHIResourcePtr mColorTexture;
	RHIResourcePtr mDepthTexture;
private:
	uint32_t mWidth = 1024;
	uint32_t mHeight = 768;
	RHITextureFormat mFormat = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
	bool m_ready = false;
	RHITextureFormat m_depth_format = RHITextureFormat::FORMAT_D24_UNORM_S8_UINT;
	bool m_is_shader_res = false;
	bool m_screen_rt = false;
};
}
