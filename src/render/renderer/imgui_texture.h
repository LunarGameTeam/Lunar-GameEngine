#pragma once

#include "core/reflection/reflection.h"
#include "render/rhi/rhi_ptr.h"
#include "render/rhi/rhi_resource.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"

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
	render::RHIViewPtr	mView;
	TRHIPtr<DX12GpuDescriptorSegment> descriptorPoolSave;
	void* mImg;
};
}

namespace luna::binding
{
template<> struct binding_converter<render::RHIResource*> : native_converter<render::RHIResource> { };
template<> struct binding_proxy<render::RHIResource> : native_binding_proxy<render::RHIResource> { };
}
