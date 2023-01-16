#include "Core/Reflection/Property.h"
#include "Core/Reflection/Type.h"
#include "Core/Reflection/Reflection.h"

namespace luna
{

RegisterTypeEmbedd_Imp(LProperty)
{
	cls->Binding<LProperty>();
	cls->BindingProperty<&LProperty::mName>("name");
	cls->BindingProperty<&LProperty::mType>("type");
	BindingModule::Luna()->AddType(cls);
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