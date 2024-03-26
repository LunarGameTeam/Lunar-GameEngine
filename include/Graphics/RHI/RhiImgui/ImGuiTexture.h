#pragma once

#include "Core/Reflection/Reflection.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIResource.h"

namespace luna
{
RegisterType(RENDER_API, graphics::RHIResource, RHIResource);
}

namespace luna::graphics
{

//IMGUI
struct RENDER_API ImguiTexture
{
public:
	RHIViewPtr	mView;
	TRHIPtr<RHIObject> mDX12Pool;
	void* mImg;
};
}

namespace luna::binding
{
template<> struct binding_converter<graphics::RHIResource*> : native_converter<graphics::RHIResource> { };
template<> struct binding_proxy<graphics::RHIResource> : native_binding_proxy<graphics::RHIResource> { };
}
