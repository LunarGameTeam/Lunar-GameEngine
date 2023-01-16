#pragma once

#include "Graphics/RHI/RHIPch.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIRenderPass.h"
#include "Graphics/RHI/RHIDescriptor.h"
#include "Graphics/RHI/RHIPtr.h"


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
