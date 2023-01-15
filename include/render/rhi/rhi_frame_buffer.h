#pragma once

#include "render/rhi/rhi_pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/rhi_render_pass.h"
#include "render/rhi/rhi_descriptor.h"
#include "render/rhi/rhi_ptr.h"


namespace luna::render
{


struct FrameBufferDesc
{
	RHIRenderPassPtr    mPass;
	LArray<RHIViewPtr> mColor;
	RHIViewPtr          mDepthStencil;
	uint32_t            mWidth  = 0;
	uint32_t            mHeight = 0;
};

class RENDER_API RHIFrameBuffer : public RHIObject
{
public:
	RHIFrameBuffer() = default;
};

}
