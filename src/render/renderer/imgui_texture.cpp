#include "imgui_texture.h"


namespace luna
{

RegisterType_Imp(render::RHIResource, RHIResource)
{

	cls->Binding<render::RHIResource>();
	BindingModule::Luna()->AddType(cls);
}

}
