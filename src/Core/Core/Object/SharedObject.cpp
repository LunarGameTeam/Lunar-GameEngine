#include "Core/Object/SharedObject.h"

namespace luna
{


RegisterTypeEmbedd_Imp(SharedObject)
{
	cls->Ctor<SharedObject>();
	cls->Binding<SharedObject>();
	BindingModule::Get("luna")->AddType(cls);
};


}