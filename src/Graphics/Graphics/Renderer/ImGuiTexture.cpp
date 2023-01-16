#include "Graphics/Renderer/ImGuiTexture.h"
#include "Graphics/RHI/DirectX12/DX12DescriptorImpl.h"


namespace luna
{

RegisterType_Imp(render::RHIResource, RHIResource)
{
	cls->Binding<render::RHIResource>();
	BindingModule::Luna()->AddType(cls);
}

}
