#include "core/reflection/property.h"
#include "core/reflection/type.h"
#include "core/reflection/reflection.h"

namespace luna
{

RegisterTypeEmbedd_Imp(LProperty)
{
	cls->Binding<LProperty>();
	cls->BindingProperty<&LProperty::mName>("name");
	cls->BindingProperty<&LProperty::mType>("type");
	LBindingModule::Luna()->AddType(cls);
}

LProperty::LProperty()
{
	mBindingDef = {};
	mBindingDef.closure = reinterpret_cast<void*>(this);
}

void LProperty::SetPropertyName(const char* name)
{
	mName = name;
	mBindingDef.name = mName.c_str();
}

}