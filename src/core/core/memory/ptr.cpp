#include "core/memory/ptr.h"

#include "core/object/base_object.h"
#include "core/memory/ptr_binding.h"
#include "core/reflection/type.h"

namespace luna
{

PPtr::PPtr(LObject *parent) :
	mParent(parent)
{
}

PPtr::~PPtr()
{
	mInstanceID = 0;
};

LObject* PPtr::Get()
{
	return LObject::InstanceIDToObject(mInstanceID);
}

void PPtr::SetPtr(LObject *val)
{
	if (val)
		mInstanceID = val->GetInstanceID();
	else
		mInstanceID = 0;
}

PPtr& PPtr::operator=(LObject *val)
{
	SetPtr(val);
	return *this;
}

}