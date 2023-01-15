#include "render/renderer/imgui_texture.h"
#include "render/rhi/DirectX12/dx12_descriptor_impl.h"


namespace luna
{

RegisterType_Imp(render::RHIResource, RHIResource)
{
	cls->Binding<render::RHIResource>();
	BindingModule::Luna()->AddType(cls);
}

}
