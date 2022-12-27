#pragma once

#include "core/reflection/reflection.h"
#include "render/rhi/rhi_ptr.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"

namespace luna
{
namespace render
{

//IMGUI
struct ImguiTexture
{
	RegisterTypeEmbedd(ImguiTexture, InvalidType);
public:
	render::RHIViewPtr	mView;
	TRHIPtr<DX12GpuDescriptorSegment> descriptorPoolSave;
	void* mImg;
};

}
namespace binding
{

template<> struct binding_converter<render::ImguiTexture*> : native_converter<render::ImguiTexture> { };
template<> struct binding_proxy<render::ImguiTexture> : native_binding_proxy<render::ImguiTexture> { };
}

}
