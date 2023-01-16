#include "Core/Memory/Ptr.h"

#include "Core/Object/BaseObject.h"
#include "Core/Memory/PtrBinding.h"
#include "Core/Reflection/Type.h"

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