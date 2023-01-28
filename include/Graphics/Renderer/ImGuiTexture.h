#pragma once

#include "Core/Reflection/Reflection.h"
#include "Graphics/RHI/RHIPtr.h"
#include "Graphics/RHI/RHIResource.h"

namespace luna
{
RegisterType(RENDER_API, render::RHIResource, RHIResource);
}

namespace luna::render
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
template<> struct binding_converter<render::RHIResource*> : native_converter<render::RHIResource> { };
template<> struct binding_proxy<render::RHIResource> : native_binding_proxy<render::RHIResource> { };
}
