#include "imgui_texture.h"

namespace luna::render
{

RegisterTypeEmbedd_Imp(ImguiTexture)
{
	cls->Binding<ImguiTexture>();
	BindingModule::Luna()->AddType(cls);
}

}
